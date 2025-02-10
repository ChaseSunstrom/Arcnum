#ifndef SPARK_MATH_HPP
#define SPARK_MATH_HPP

#include "spark_math_types.hpp"

namespace spark::math {
	// Constants
	constexpr f32 k_pi = 3.14159265358979323846f;
	constexpr f32 k_epsilon = 1e-6f;
	constexpr f32 k_two_pi = 2.0f * k_pi;
	constexpr f32 k_half_pi = 0.5f * k_pi;
	constexpr f32 k_e = 2.71828182845904523536f;
	constexpr f32 k_log2e = 1.44269504088896340736f;
	constexpr f32 k_log10e = 0.434294481903251827651f;
	constexpr f32 k_ln2 = 0.693147180559945309417f;
	constexpr f32 k_ln10 = 2.30258509299404568402f;
	constexpr f32 k_sqrt2 = 1.41421356237309504880f;
	constexpr f32 k_sqrt1_2 = 0.707106781186547524401f;

	template<typename T> inline T         Sin(T x) { return std::sin(x); }
	template<typename T> inline Vec<T, 2> Sin(const Vec<T, 2>& v) { return Vec<T, 2>(Sin(v.x), Sin(v.y)); }
	template<typename T> inline Vec<T, 3> Sin(const Vec<T, 3>& v) { return Vec<T, 3>(Sin(v.x), Sin(v.y), Sin(v.z)); }
	template<typename T> inline Vec<T, 4> Sin(const Vec<T, 4>& v) { return Vec<T, 4>(Sin(v.x), Sin(v.y), Sin(v.z), Sin(v.w)); }

	template<typename T> inline T         Cos(T x) { return std::cos(x); }
	template<typename T> inline Vec<T, 2> Cos(const Vec<T, 2>& v) { return Vec<T, 2>(Cos(v.x), Cos(v.y)); }
	template<typename T> inline Vec<T, 3> Cos(const Vec<T, 3>& v) { return Vec<T, 3>(Cos(v.x), Cos(v.y), Cos(v.z)); }
	template<typename T> inline Vec<T, 4> Cos(const Vec<T, 4>& v) { return Vec<T, 4>(Cos(v.x), Cos(v.y), Cos(v.z), Cos(v.w)); }

	template<typename T> inline T         Tan(T x) { return std::tan(x); }
	template<typename T> inline Vec<T, 2> Tan(const Vec<T, 2>& v) { return Vec<T, 2>(Tan(v.x), Tan(v.y)); }
	template<typename T> inline Vec<T, 3> Tan(const Vec<T, 3>& v) { return Vec<T, 3>(Tan(v.x), Tan(v.y), Tan(v.z)); }
	template<typename T> inline Vec<T, 4> Tan(const Vec<T, 4>& v) { return Vec<T, 4>(Tan(v.x), Tan(v.y), Tan(v.z), Tan(v.w)); }

	// Arc trigonometric functions
	template<typename T> inline T         Asin(T x) { return std::asin(x); }
	template<typename T> inline Vec<T, 2> Asin(const Vec<T, 2>& v) { return Vec<T, 2>(Asin(v.x), Asin(v.y)); }
	template<typename T> inline Vec<T, 3> Asin(const Vec<T, 3>& v) { return Vec<T, 3>(Asin(v.x), Asin(v.y), Asin(v.z)); }
	template<typename T> inline Vec<T, 4> Asin(const Vec<T, 4>& v) { return Vec<T, 4>(Asin(v.x), Asin(v.y), Asin(v.z), Asin(v.w)); }

	template<typename T> inline T         Acos(T x) { return std::acos(x); }
	template<typename T> inline Vec<T, 2> Acos(const Vec<T, 2>& v) { return Vec<T, 2>(Acos(v.x), Acos(v.y)); }
	template<typename T> inline Vec<T, 3> Acos(const Vec<T, 3>& v) { return Vec<T, 3>(Acos(v.x), Acos(v.y), Acos(v.z)); }
	template<typename T> inline Vec<T, 4> Acos(const Vec<T, 4>& v) { return Vec<T, 4>(Acos(v.x), Acos(v.y), Acos(v.z), Acos(v.w)); }

	template<typename T> inline T         Atan(T x) { return std::atan(x); }
	template<typename T> inline Vec<T, 2> Atan(const Vec<T, 2>& v) { return Vec<T, 2>(Atan(v.x), Atan(v.y)); }
	template<typename T> inline Vec<T, 3> Atan(const Vec<T, 3>& v) { return Vec<T, 3>(Atan(v.x), Atan(v.y), Atan(v.z)); }
	template<typename T> inline Vec<T, 4> Atan(const Vec<T, 4>& v) { return Vec<T, 4>(Atan(v.x), Atan(v.y), Atan(v.z), Atan(v.w)); }

	template<typename T> inline T         Atan2(T y, T x) { return std::atan2(y, x); }
	template<typename T> inline Vec<T, 2> Atan2(const Vec<T, 2>& y, const Vec<T, 2>& x) { return Vec<T, 2>(Atan2(y.x, x.x), Atan2(y.y, x.y)); }
	template<typename T> inline Vec<T, 3> Atan2(const Vec<T, 3>& y, const Vec<T, 3>& x) { return Vec<T, 3>(Atan2(y.x, x.x), Atan2(y.y, x.y), Atan2(y.z, x.z)); }
	template<typename T> inline Vec<T, 4> Atan2(const Vec<T, 4>& y, const Vec<T, 4>& x) { return Vec<T, 4>(Atan2(y.x, x.x), Atan2(y.y, x.y), Atan2(y.z, x.z), Atan2(y.w, x.w)); }

	// Hyperbolic functions
	template<typename T> inline T         Sinh(T x) { return std::sinh(x); }
	template<typename T> inline Vec<T, 2> Sinh(const Vec<T, 2>& v) { return Vec<T, 2>(Sinh(v.x), Sinh(v.y)); }
	template<typename T> inline Vec<T, 3> Sinh(const Vec<T, 3>& v) { return Vec<T, 3>(Sinh(v.x), Sinh(v.y), Sinh(v.z)); }
	template<typename T> inline Vec<T, 4> Sinh(const Vec<T, 4>& v) { return Vec<T, 4>(Sinh(v.x), Sinh(v.y), Sinh(v.z), Sinh(v.w)); }

	template<typename T> inline T         Cosh(T x) { return std::cosh(x); }
	template<typename T> inline Vec<T, 2> Cosh(const Vec<T, 2>& v) { return Vec<T, 2>(Cosh(v.x), Cosh(v.y)); }
	template<typename T> inline Vec<T, 3> Cosh(const Vec<T, 3>& v) { return Vec<T, 3>(Cosh(v.x), Cosh(v.y), Cosh(v.z)); }
	template<typename T> inline Vec<T, 4> Cosh(const Vec<T, 4>& v) { return Vec<T, 4>(Cosh(v.x), Cosh(v.y), Cosh(v.z), Cosh(v.w)); }

	template<typename T> inline T         Tanh(T x) { return std::tanh(x); }
	template<typename T> inline Vec<T, 2> Tanh(const Vec<T, 2>& v) { return Vec<T, 2>(Tanh(v.x), Tanh(v.y)); }
	template<typename T> inline Vec<T, 3> Tanh(const Vec<T, 3>& v) { return Vec<T, 3>(Tanh(v.x), Tanh(v.y), Tanh(v.z)); }
	template<typename T> inline Vec<T, 4> Tanh(const Vec<T, 4>& v) { return Vec<T, 4>(Tanh(v.x), Tanh(v.y), Tanh(v.z), Tanh(v.w)); }

	// Inverse hyperbolic functions
	template<typename T> inline T         Asinh(T x) { return std::asinh(x); }
	template<typename T> inline Vec<T, 2> Asinh(const Vec<T, 2>& v) { return Vec<T, 2>(Asinh(v.x), Asinh(v.y)); }
	template<typename T> inline Vec<T, 3> Asinh(const Vec<T, 3>& v) { return Vec<T, 3>(Asinh(v.x), Asinh(v.y), Asinh(v.z)); }
	template<typename T> inline Vec<T, 4> Asinh(const Vec<T, 4>& v) { return Vec<T, 4>(Asinh(v.x), Asinh(v.y), Asinh(v.z), Asinh(v.w)); }

	template<typename T> inline T         Acosh(T x) { return std::acosh(x); }
	template<typename T> inline Vec<T, 2> Acosh(const Vec<T, 2>& v) { return Vec<T, 2>(Acosh(v.x), Acosh(v.y)); }
	template<typename T> inline Vec<T, 3> Acosh(const Vec<T, 3>& v) { return Vec<T, 3>(Acosh(v.x), Acosh(v.y), Acosh(v.z)); }
	template<typename T> inline Vec<T, 4> Acosh(const Vec<T, 4>& v) { return Vec<T, 4>(Acosh(v.x), Acosh(v.y), Acosh(v.z), Acosh(v.w)); }

	template<typename T> inline T         Atanh(T x) { return std::atanh(x); }
	template<typename T> inline Vec<T, 2> Atanh(const Vec<T, 2>& v) { return Vec<T, 2>(Atanh(v.x), Atanh(v.y)); }
	template<typename T> inline Vec<T, 3> Atanh(const Vec<T, 3>& v) { return Vec<T, 3>(Atanh(v.x), Atanh(v.y), Atanh(v.z)); }
	template<typename T> inline Vec<T, 4> Atanh(const Vec<T, 4>& v) { return Vec<T, 4>(Atanh(v.x), Atanh(v.y), Atanh(v.z), Atanh(v.w)); }

	// Additional trigonometric utilities
	template<typename T> inline void SinCos(T x, T& sin_x, T& cos_x) {
		sin_x = Sin(x);
		cos_x = Cos(x);
	}

	template<typename T> inline Vec<T, 2> SinCos(T x) { return Vec<T, 2>(Sin(x), Cos(x)); }

	// Degrees and Radians conversion for vectors
	template<typename T> inline Vec<T, 2> Degrees(const Vec<T, 2>& radians) { return Vec<T, 2>(RadiansToDegrees(radians.x), RadiansToDegrees(radians.y)); }
	template<typename T> inline Vec<T, 3> Degrees(const Vec<T, 3>& radians) { return Vec<T, 3>(RadiansToDegrees(radians.x), RadiansToDegrees(radians.y), RadiansToDegrees(radians.z)); }
	template<typename T> inline Vec<T, 4> Degrees(const Vec<T, 4>& radians) {
		return Vec<T, 4>(RadiansToDegrees(radians.x), RadiansToDegrees(radians.y), RadiansToDegrees(radians.z), RadiansToDegrees(radians.w));
	}

	template<typename T> inline Vec<T, 2> Radians(const Vec<T, 2>& degrees) { return Vec<T, 2>(DegreesToRadians(degrees.x), DegreesToRadians(degrees.y)); }
	template<typename T> inline Vec<T, 3> Radians(const Vec<T, 3>& degrees) { return Vec<T, 3>(DegreesToRadians(degrees.x), DegreesToRadians(degrees.y), DegreesToRadians(degrees.z)); }
	template<typename T> inline Vec<T, 4> Radians(const Vec<T, 4>& degrees) {
		return Vec<T, 4>(DegreesToRadians(degrees.x), DegreesToRadians(degrees.y), DegreesToRadians(degrees.z), DegreesToRadians(degrees.w));
	}
	// Basic math functions
	template<typename T> inline T         Floor(T x) { return std::floor(x); }
	template<typename T> inline Vec<T, 2> Floor(const Vec<T, 2>& v) { return Vec<T, 2>(Floor(v.x), Floor(v.y)); }
	template<typename T> inline Vec<T, 3> Floor(const Vec<T, 3>& v) { return Vec<T, 3>(Floor(v.x), Floor(v.y), Floor(v.z)); }
	template<typename T> inline Vec<T, 4> Floor(const Vec<T, 4>& v) { return Vec<T, 4>(Floor(v.x), Floor(v.y), Floor(v.z), Floor(v.w)); }

	template<typename T> inline T         Ceil(T x) { return std::ceil(x); }
	template<typename T> inline Vec<T, 2> Ceil(const Vec<T, 2>& v) { return Vec<T, 2>(Ceil(v.x), Ceil(v.y)); }
	template<typename T> inline Vec<T, 3> Ceil(const Vec<T, 3>& v) { return Vec<T, 3>(Ceil(v.x), Ceil(v.y), Ceil(v.z)); }
	template<typename T> inline Vec<T, 4> Ceil(const Vec<T, 4>& v) { return Vec<T, 4>(Ceil(v.x), Ceil(v.y), Ceil(v.z), Ceil(v.w)); }

	template<typename T> inline T         Round(T x) { return std::round(x); }
	template<typename T> inline Vec<T, 2> Round(const Vec<T, 2>& v) { return Vec<T, 2>(Round(v.x), Round(v.y)); }
	template<typename T> inline Vec<T, 3> Round(const Vec<T, 3>& v) { return Vec<T, 3>(Round(v.x), Round(v.y), Round(v.z)); }
	template<typename T> inline Vec<T, 4> Round(const Vec<T, 4>& v) { return Vec<T, 4>(Round(v.x), Round(v.y), Round(v.z), Round(v.w)); }

	template<typename T> inline T         Trunc(T x) { return std::trunc(x); }
	template<typename T> inline Vec<T, 2> Trunc(const Vec<T, 2>& v) { return Vec<T, 2>(Trunc(v.x), Trunc(v.y)); }
	template<typename T> inline Vec<T, 3> Trunc(const Vec<T, 3>& v) { return Vec<T, 3>(Trunc(v.x), Trunc(v.y), Trunc(v.z)); }
	template<typename T> inline Vec<T, 4> Trunc(const Vec<T, 4>& v) { return Vec<T, 4>(Trunc(v.x), Trunc(v.y), Trunc(v.z), Trunc(v.w)); }

	template<typename T> inline T         Fract(T x) { return x - Floor(x); }
	template<typename T> inline Vec<T, 2> Fract(const Vec<T, 2>& v) { return Vec<T, 2>(Fract(v.x), Fract(v.y)); }
	template<typename T> inline Vec<T, 3> Fract(const Vec<T, 3>& v) { return Vec<T, 3>(Fract(v.x), Fract(v.y), Fract(v.z)); }
	template<typename T> inline Vec<T, 4> Fract(const Vec<T, 4>& v) { return Vec<T, 4>(Fract(v.x), Fract(v.y), Fract(v.z), Fract(v.w)); }

	template<typename T> inline T         Mod(T x, T y) { return x - y * Floor(x / y); }
	template<typename T> inline Vec<T, 2> Mod(const Vec<T, 2>& x, const Vec<T, 2>& y) { return Vec<T, 2>(Mod(x.x, y.x), Mod(x.y, y.y)); }
	template<typename T> inline Vec<T, 3> Mod(const Vec<T, 3>& x, const Vec<T, 3>& y) { return Vec<T, 3>(Mod(x.x, y.x), Mod(x.y, y.y), Mod(x.z, y.z)); }
	template<typename T> inline Vec<T, 4> Mod(const Vec<T, 4>& x, const Vec<T, 4>& y) { return Vec<T, 4>(Mod(x.x, y.x), Mod(x.y, y.y), Mod(x.z, y.z), Mod(x.w, y.w)); }

	template<typename T> inline T         Abs(T x) { return std::abs(x); }
	template<typename T> inline Vec<T, 2> Abs(const Vec<T, 2>& v) { return Vec<T, 2>(Abs(v.x), Abs(v.y)); }
	template<typename T> inline Vec<T, 3> Abs(const Vec<T, 3>& v) { return Vec<T, 3>(Abs(v.x), Abs(v.y), Abs(v.z)); }
	template<typename T> inline Vec<T, 4> Abs(const Vec<T, 4>& v) { return Vec<T, 4>(Abs(v.x), Abs(v.y), Abs(v.z), Abs(v.w)); }

	// Exponential functions
	template<typename T> inline T         Exp(T x) { return std::exp(x); }
	template<typename T> inline Vec<T, 2> Exp(const Vec<T, 2>& v) { return Vec<T, 2>(Exp(v.x), Exp(v.y)); }
	template<typename T> inline Vec<T, 3> Exp(const Vec<T, 3>& v) { return Vec<T, 3>(Exp(v.x), Exp(v.y), Exp(v.z)); }
	template<typename T> inline Vec<T, 4> Exp(const Vec<T, 4>& v) { return Vec<T, 4>(Exp(v.x), Exp(v.y), Exp(v.z), Exp(v.w)); }

	template<typename T> inline T         Log(T x) { return std::log(x); }
	template<typename T> inline Vec<T, 2> Log(const Vec<T, 2>& v) { return Vec<T, 2>(Log(v.x), Log(v.y)); }
	template<typename T> inline Vec<T, 3> Log(const Vec<T, 3>& v) { return Vec<T, 3>(Log(v.x), Log(v.y), Log(v.z)); }
	template<typename T> inline Vec<T, 4> Log(const Vec<T, 4>& v) { return Vec<T, 4>(Log(v.x), Log(v.y), Log(v.z), Log(v.w)); }

	template<typename T> inline T         Exp2(T x) { return std::exp2(x); }
	template<typename T> inline Vec<T, 2> Exp2(const Vec<T, 2>& v) { return Vec<T, 2>(Exp2(v.x), Exp2(v.y)); }
	template<typename T> inline Vec<T, 3> Exp2(const Vec<T, 3>& v) { return Vec<T, 3>(Exp2(v.x), Exp2(v.y), Exp2(v.z)); }
	template<typename T> inline Vec<T, 4> Exp2(const Vec<T, 4>& v) { return Vec<T, 4>(Exp2(v.x), Exp2(v.y), Exp2(v.z), Exp2(v.w)); }

	template<typename T> inline T         Log2(T x) { return std::log2(x); }
	template<typename T> inline Vec<T, 2> Log2(const Vec<T, 2>& v) { return Vec<T, 2>(Log2(v.x), Log2(v.y)); }
	template<typename T> inline Vec<T, 3> Log2(const Vec<T, 3>& v) { return Vec<T, 3>(Log2(v.x), Log2(v.y), Log2(v.z)); }
	template<typename T> inline Vec<T, 4> Log2(const Vec<T, 4>& v) { return Vec<T, 4>(Log2(v.x), Log2(v.y), Log2(v.z), Log2(v.w)); }

	// Power functions
	template<typename T> inline T         Sqrt(T x) { return std::sqrt(x); }
	template<typename T> inline Vec<T, 2> Sqrt(const Vec<T, 2>& v) { return Vec<T, 2>(Sqrt(v.x), Sqrt(v.y)); }
	template<typename T> inline Vec<T, 3> Sqrt(const Vec<T, 3>& v) { return Vec<T, 3>(Sqrt(v.x), Sqrt(v.y), Sqrt(v.z)); }
	template<typename T> inline Vec<T, 4> Sqrt(const Vec<T, 4>& v) { return Vec<T, 4>(Sqrt(v.x), Sqrt(v.y), Sqrt(v.z), Sqrt(v.w)); }

	template<typename T> inline T         InverseSqrt(T x) { return T{ 1 } / Sqrt(x); }
	template<typename T> inline Vec<T, 2> InverseSqrt(const Vec<T, 2>& v) { return Vec<T, 2>(InverseSqrt(v.x), InverseSqrt(v.y)); }
	template<typename T> inline Vec<T, 3> InverseSqrt(const Vec<T, 3>& v) { return Vec<T, 3>(InverseSqrt(v.x), InverseSqrt(v.y), InverseSqrt(v.z)); }
	template<typename T> inline Vec<T, 4> InverseSqrt(const Vec<T, 4>& v) { return Vec<T, 4>(InverseSqrt(v.x), InverseSqrt(v.y), InverseSqrt(v.z), InverseSqrt(v.w)); }

	// Additional utility functions
	template<typename T> inline T         Sign(T x) { return (T{ 0 } < x) - (x < T{ 0 }); }
	template<typename T> inline Vec<T, 2> Sign(const Vec<T, 2>& v) { return Vec<T, 2>(Sign(v.x), Sign(v.y)); }
	template<typename T> inline Vec<T, 3> Sign(const Vec<T, 3>& v) { return Vec<T, 3>(Sign(v.x), Sign(v.y), Sign(v.z)); }
	template<typename T> inline Vec<T, 4> Sign(const Vec<T, 4>& v) { return Vec<T, 4>(Sign(v.x), Sign(v.y), Sign(v.z), Sign(v.w)); }

	template<typename T> inline T         Step(T edge, T x) { return x < edge ? T{ 0 } : T{ 1 }; }
	template<typename T> inline Vec<T, 2> Step(const Vec<T, 2>& edge, const Vec<T, 2>& x) { return Vec<T, 2>(Step(edge.x, x.x), Step(edge.y, x.y)); }
	template<typename T> inline Vec<T, 3> Step(const Vec<T, 3>& edge, const Vec<T, 3>& x) { return Vec<T, 3>(Step(edge.x, x.x), Step(edge.y, x.y), Step(edge.z, x.z)); }
	template<typename T> inline Vec<T, 4> Step(const Vec<T, 4>& edge, const Vec<T, 4>& x) {
		return Vec<T, 4>(Step(edge.x, x.x), Step(edge.y, x.y), Step(edge.z, x.z), Step(edge.w, x.w));
	}


	template<typename T> inline T* ValuePtr(Vec<T, 2>& v) { return &(v[0]); }

	template<typename T> inline const T* ValuePtr(const Vec<T, 2>& v) { return &(v[0]); }

	template<typename T> inline T* ValuePtr(Vec<T, 3>& v) { return &(v[0]); }

	template<typename T> inline const T* ValuePtr(const Vec<T, 3>& v) { return &(v[0]); }

	template<typename T> inline T* ValuePtr(Vec<T, 4>& v) { return &(v[0]); }

	template<typename T> inline const T* ValuePtr(const Vec<T, 4>& v) { return &(v[0]); }

	template<typename T> inline T* ValuePtr(Quaternion<T>& q) { return &(q.x); }

	template<typename T> inline const T* ValuePtr(const Quaternion<T>& q) { return &(q.x); }

	template<typename T, usize R, usize C> inline T* ValuePtr(Mat<T, R, C>& m) { return &(m(0, 0)); }

	template<typename T, usize R, usize C> inline const T* ValuePtr(const Mat<T, R, C>& m) { return &(m(0, 0)); }

	// Make functions for creating vectors/matrices from raw pointers
	template<typename T> inline Vec<T, 2> MakeVec2(const T* ptr) { return Vec<T, 2>(ptr[0], ptr[1]); }

	template<typename T> inline Vec<T, 3> MakeVec3(const T* ptr) { return Vec<T, 3>(ptr[0], ptr[1], ptr[2]); }

	template<typename T> inline Vec<T, 4> MakeVec4(const T* ptr) { return Vec<T, 4>(ptr[0], ptr[1], ptr[2], ptr[3]); }

	template<typename T> inline Quaternion<T> MakeQuat(const T* ptr) { return Quaternion<T>(ptr[0], ptr[1], ptr[2], ptr[3]); }

	template<typename T> inline Mat<T, 2, 2> MakeMat2(const T* ptr) {
		Mat<T, 2, 2> result;
		for (usize i = 0; i < 4; ++i) {
			result(i / 2, i % 2) = ptr[i];
		}
		return result;
	}

	template<typename T> inline Mat<T, 3, 3> MakeMat3(const T* ptr) {
		Mat<T, 3, 3> result;
		for (usize i = 0; i < 9; ++i) {
			result(i / 3, i % 3) = ptr[i];
		}
		return result;
	}

	template<typename T> inline Mat<T, 4, 4> MakeMat4(const T* ptr) {
		Mat<T, 4, 4> result;
		for (usize i = 0; i < 16; ++i) {
			result(i / 4, i % 4) = ptr[i];
		}
		return result;
	}

	// Matrix casting functions
	template<typename T> inline Mat<T, 4, 4> Mat4Cast(const Mat<T, 3, 3>& m) {
		Mat<T, 4, 4> result = Mat<T, 4, 4>::Identity();
		for (usize i = 0; i < 3; ++i)
			for (usize j = 0; j < 3; ++j)
				result(i, j) = m(i, j);
		return result;
	}

	template<typename T> inline Mat<T, 3, 3> Mat3Cast(const Mat<T, 4, 4>& m) {
		Mat<T, 3, 3> result;
		for (usize i = 0; i < 3; ++i)
			for (usize j = 0; j < 3; ++j)
				result(i, j) = m(i, j);
		return result;
	}

	// Quaternion to Matrix conversions
	template<typename T> inline Mat<T, 3, 3> Mat3Cast(const Quaternion<T>& q) {
		Mat<T, 3, 3> result;
		T            xx = q.x * q.x;
		T            yy = q.y * q.y;
		T            zz = q.z * q.z;
		T            xy = q.x * q.y;
		T            xz = q.x * q.z;
		T            yz = q.y * q.z;
		T            wx = q.w * q.x;
		T            wy = q.w * q.y;
		T            wz = q.w * q.z;

		result(0, 0) = T{ 1 } - T{ 2 } *(yy + zz);
		result(0, 1) = T{ 2 } *(xy - wz);
		result(0, 2) = T{ 2 } *(xz + wy);

		result(1, 0) = T{ 2 } *(xy + wz);
		result(1, 1) = T{ 1 } - T{ 2 } *(xx + zz);
		result(1, 2) = T{ 2 } *(yz - wx);

		result(2, 0) = T{ 2 } *(xz - wy);
		result(2, 1) = T{ 2 } *(yz + wx);
		result(2, 2) = T{ 1 } - T{ 2 } *(xx + yy);

		return result;
	}

	template<typename T> inline Mat<T, 4, 4> Mat4Cast(const Quaternion<T>& q) { return Mat4Cast(Mat3Cast(q)); }

	// Matrix to Quaternion conversion
	template<typename T> inline Quaternion<T> QuatCast(const Mat<T, 3, 3>& m) {
		T four_x_squared_minus1 = m(0, 0) - m(1, 1) - m(2, 2);
		T four_y_squared_minus1 = m(1, 1) - m(0, 0) - m(2, 2);
		T for_z_squared_minus1 = m(2, 2) - m(0, 0) - m(1, 1);
		T for_w_squared_minus1 = m(0, 0) + m(1, 1) + m(2, 2);

		i32 biggest_index = 0;
		T   for_biggest_squared_minus1 = for_w_squared_minus1;
		if (four_x_squared_minus1 > for_biggest_squared_minus1) {
			for_biggest_squared_minus1 = four_x_squared_minus1;
			biggest_index = 1;
		}
		if (four_y_squared_minus1 > for_biggest_squared_minus1) {
			for_biggest_squared_minus1 = four_y_squared_minus1;
			biggest_index = 2;
		}
		if (for_z_squared_minus1 > for_biggest_squared_minus1) {
			for_biggest_squared_minus1 = for_z_squared_minus1;
			biggest_index = 3;
		}

		T biggest_val = Sqrt(for_biggest_squared_minus1 + T{ 1 }) * T { 0.5 };
		T mult = T{ 0.25 } / biggest_val;

		Quaternion<T> result;
		switch (biggest_index) {
		case 0:
			result.w = biggest_val;
			result.x = (m(2, 1) - m(1, 2)) * mult;
			result.y = (m(0, 2) - m(2, 0)) * mult;
			result.z = (m(1, 0) - m(0, 1)) * mult;
			break;
		case 1:
			result.w = (m(2, 1) - m(1, 2)) * mult;
			result.x = biggest_val;
			result.y = (m(1, 0) + m(0, 1)) * mult;
			result.z = (m(0, 2) + m(2, 0)) * mult;
			break;
		case 2:
			result.w = (m(0, 2) - m(2, 0)) * mult;
			result.x = (m(1, 0) + m(0, 1)) * mult;
			result.y = biggest_val;
			result.z = (m(2, 1) + m(1, 2)) * mult;
			break;
		case 3:
			result.w = (m(1, 0) - m(0, 1)) * mult;
			result.x = (m(0, 2) + m(2, 0)) * mult;
			result.y = (m(2, 1) + m(1, 2)) * mult;
			result.z = biggest_val;
			break;
		}
		return result;
	}

	template<typename T> inline Quaternion<T> QuatCast(const Mat<T, 4, 4>& m) { return QuatCast(Mat3Cast(m)); }

	// Vector casting functions
	template<typename T> inline Vec<T, 2> Vec2Cast(const Vec<T, 3>& v) { return Vec<T, 2>(v.x, v.y); }

	template<typename T> inline Vec<T, 2> Vec2Cast(const Vec<T, 4>& v) { return Vec<T, 2>(v.x, v.y); }

	template<typename T> inline Vec<T, 3> Vec3Cast(const Vec<T, 2>& v, T z = T{ 0 }) { return Vec<T, 3>(v.x, v.y, z); }

	template<typename T> inline Vec<T, 3> Vec3Cast(const Vec<T, 4>& v) { return Vec<T, 3>(v.x, v.y, v.z); }

	template<typename T> inline Vec<T, 4> Vec4Cast(const Vec<T, 2>& v, T z = T{ 0 }, T w = T{ 1 }) { return Vec<T, 4>(v.x, v.y, z, w); }

	template<typename T> inline Vec<T, 4> Vec4Cast(const Vec<T, 3>& v, T w = T{ 1 }) { return Vec<T, 4>(v.x, v.y, v.z, w); }


	// Helper functions for copying to pointer
	template<typename T> inline void CopyTo(const Vec<T, 2>& v, T* ptr) {
		ptr[0] = v.x;
		ptr[1] = v.y;
	}

	template<typename T> inline void CopyTo(const Vec<T, 3>& v, T* ptr) {
		ptr[0] = v.x;
		ptr[1] = v.y;
		ptr[2] = v.z;
	}

	template<typename T> inline void CopyTo(const Vec<T, 4>& v, T* ptr) {
		ptr[0] = v.x;
		ptr[1] = v.y;
		ptr[2] = v.z;
		ptr[3] = v.w;
	}

	template<typename T> inline void CopyTo(const Quaternion<T>& q, T* ptr) {
		ptr[0] = q.x;
		ptr[1] = q.y;
		ptr[2] = q.z;
		ptr[3] = q.w;
	}

	template<typename T> inline void CopyTo(const Mat<T, 2, 2>& m, T* ptr) {
		for (usize i = 0; i < 4; ++i) {
			ptr[i] = m(i / 2, i % 2);
		}
	}

	template<typename T> inline void CopyTo(const Mat<T, 3, 3>& m, T* ptr) {
		for (usize i = 0; i < 9; ++i) {
			ptr[i] = m(i / 3, i % 3);
		}
	}

	template<typename T> inline void CopyTo(const Mat<T, 4, 4>& m, T* ptr) {
		for (usize i = 0; i < 16; ++i) {
			ptr[i] = m(i / 4, i % 4);
		}
	}

	// Basic trigonometric functions

	template<typename T> inline T Mix(T x, T y, T a) { return x * (T{ 1 } - a) + y * a; }

	template<typename T> inline Vec<T, 2> Mix(const Vec<T, 2>& x, const Vec<T, 2>& y, T a) { return x * (T{ 1 } - a) + y * a; }

	template<typename T> inline Vec<T, 3> Mix(const Vec<T, 3>& x, const Vec<T, 3>& y, T a) { return x * (T{ 1 } - a) + y * a; }

	template<typename T> inline Vec<T, 4> Mix(const Vec<T, 4>& x, const Vec<T, 4>& y, T a) { return x * (T{ 1 } - a) + y * a; }

	template<typename T, usize N> inline T Length(const Vec<T, N>& v) { return v.Length(); }

	// Get the squared length of a vector (faster than Length() when only comparing distances)
	template<typename T, usize N> inline T LengthSquared(const Vec<T, N>& v) { return v.LengthSquared(); }

	// Get the Euclidean distance between two points
	template<typename T, usize N> inline T Distance(const Vec<T, N>& a, const Vec<T, N>& b) { return Length(b - a); }

	// Get the squared distance between two points (faster than Distance())
	template<typename T, usize N> inline T DistanceSquared(const Vec<T, N>& a, const Vec<T, N>& b) { return LengthSquared(b - a); }

	// Get the Manhattan (L1) distance between two points
	template<typename T, usize N> inline T ManhattanDistance(const Vec<T, N>& a, const Vec<T, N>& b) {
		T sum = T{ 0 };
		for (usize i = 0; i < N; ++i) {
			sum += Abs(b[i] - a[i]);
		}
		return sum;
	}

	// Get the Chebyshev (L∞) distance between two points
	template<typename T, usize N> inline T ChebyshevDistance(const Vec<T, N>& a, const Vec<T, N>& b) {
		T max_diff = T{ 0 };
		for (usize i = 0; i < N; ++i) {
			max_diff = Max(max_diff, Abs(b[i] - a[i]));
		}
		return max_diff;
	}

	// Normalize a vector (returns a vector in the same direction with length 1)
	template<typename T, usize N> inline Vec<T, N> Normalize(const Vec<T, N>& v) { return v.Normalized(); }

	// Check if a vector has (approximately) unit length
	template<typename T, usize N> inline bool IsNormalized(const Vec<T, N>& v, T epsilon = T{ 1e-6 }) { return Abs(LengthSquared(v) - T{ 1 }) < epsilon; }

	// Get the projection length of vector a onto vector b
	template<typename T, usize N> inline T ProjectionLength(const Vec<T, N>& a, const Vec<T, N>& b) { return Dot(a, Normalize(b)); }

	// Get the perpendicular distance from point p to line defined by points a and b
	template<typename T, usize N> inline T PointLineDistance(const Vec<T, N>& p, const Vec<T, N>& a, const Vec<T, N>& b) {
		Vec<T, N> ab = b - a;
		Vec<T, N> ap = p - a;
		T         proj_length = ProjectionLength(ap, ab);
		return Sqrt(LengthSquared(ap) - proj_length * proj_length);
	}

	// Integer functions
	template<typename T> inline T IsPowerOfTwo(T x) { return x > 0 && (x & (x - 1)) == 0; }

	template<typename T> inline T NextPowerOfTwo(T x) {
		x--;
		x |= x >> 1;
		x |= x >> 2;
		x |= x >> 4;
		x |= x >> 8;
		x |= x >> 16;
		return x + 1;
	}

	template<typename T> inline T PrevPowerOfTwo(T x) {
		x |= x >> 1;
		x |= x >> 2;
		x |= x >> 4;
		x |= x >> 8;
		x |= x >> 16;
		return x - (x >> 1);
	}

	// Random number generation
	inline f32 Random() {
		static std::random_device                  rd;
		static std::mt19937                        gen(rd());
		static std::uniform_real_distribution<f32> dis(0.0f, 1.0f);
		return dis(gen);
	}

	template<typename T> inline T RandomRange(T min_val, T max_val) { return min_val + (max_val - min_val) * Random(); }

	template<typename T> inline Vec<T, 3> RandomUnitVector() {
		T z = RandomRange(T{ -1 }, T{ 1 });
		T theta = RandomRange(T{ 0 }, T{ k_two_pi });
		T r = Sqrt(T{ 1 } - z * z);
		return Vec<T, 3>(r * Cos(theta), r * Sin(theta), z);
	}

	// Basic arithmetic functions
	template<typename T> inline T Min(T a, T b) { return (a < b) ? a : b; }

	template<typename T> inline T Max(T a, T b) { return (a > b) ? a : b; }

	template<typename T> inline T Clamp(T value, T min_val, T max_val) { return Min(Max(value, min_val), max_val); }

	// Angle conversion
	template<typename T> inline T DegreesToRadians(T degrees) { return degrees * (T{ k_pi } / T{ 180 }); }

	template<typename T> inline T RadiansToDegrees(T radians) { return radians * (T{ 180 } / T{ k_pi }); }

	// Vector operations
	template<typename T, usize N> inline T Dot(const Vec<T, N>& a, const Vec<T, N>& b) { return a.Dot(b); }

	template<typename T> inline Vec<T, 3> Cross(const Vec<T, 3>& a, const Vec<T, 3>& b) { return a.Cross(b); }

	// Quaternion operations
	template<typename T> inline Quaternion<T> Slerp(const Quaternion<T>& a, const Quaternion<T>& b, T t) {
		T cos_theta = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;

		if (cos_theta < T{ 0 }) {
			cos_theta = -cos_theta;
			Quaternion<T> temp(-b.x, -b.y, -b.z, -b.w);
			return NLerp(a, temp, t);
		}

		if (cos_theta > T{ 0.9995 }) {
			return NLerp(a, b, t);
		}

		T theta = Acos(cos_theta);
		T sin_theta = Sin(theta);
		T angle = theta * t;
		T scale1 = Sin(theta - angle) / sin_theta;
		T scale2 = Sin(angle) / sin_theta;

		return Quaternion<T>(scale1 * a.x + scale2 * b.x, scale1 * a.y + scale2 * b.y, scale1 * a.z + scale2 * b.z, scale1 * a.w + scale2 * b.w);
	}

	template<typename T> inline Quaternion<T> NLerp(const Quaternion<T>& a, const Quaternion<T>& b, T t) {
		Quaternion<T> result(a.x * (T{ 1 } - t) + b.x * t, a.y * (T{ 1 } - t) + b.y * t, a.z * (T{ 1 } - t) + b.z * t, a.w * (T{ 1 } - t) + b.w * t);
		return result.Normalized();
	}

	// Matrix operations
	template<typename T> inline Mat<T, 4, 4> CreateTranslationMatrix(const Vec<T, 3>& translation) {
		Mat<T, 4, 4> result = Mat<T, 4, 4>::Identity();
		result(3, 0) = translation.x;
		result(3, 1) = translation.y;
		result(3, 2) = translation.z;
		return result;
	}

	template<typename T> inline Mat<T, 4, 4> CreateScaleMatrix(const Vec<T, 3>& scale) {
		Mat<T, 4, 4> result = Mat<T, 4, 4>::Identity();
		result(0, 0) = scale.x;
		result(1, 1) = scale.y;
		result(2, 2) = scale.z;
		return result;
	}

	template<typename T> inline Mat<T, 4, 4> CreateRotationMatrix(const Vec<T, 3>& euler_angles) {
		T cx = Cos(euler_angles.x);
		T sx = Sin(euler_angles.x);
		T cy = Cos(euler_angles.y);
		T sy = Sin(euler_angles.y);
		T cz = Cos(euler_angles.z);
		T sz = Sin(euler_angles.z);

		Mat<T, 4, 4> result = Mat<T, 4, 4>::Identity();

		result(0, 0) = cy * cz;
		result(0, 1) = cy * sz;
		result(0, 2) = -sy;

		result(1, 0) = sx * sy * cz - cx * sz;
		result(1, 1) = sx * sy * sz + cx * cz;
		result(1, 2) = sx * cy;

		result(2, 0) = cx * sy * cz + sx * sz;
		result(2, 1) = cx * sy * sz - sx * cz;
		result(2, 2) = cx * cy;

		return result;
	}

	template<typename T> inline Vec<T, 3> Reflect(const Vec<T, 3>& incident, const Vec<T, 3>& normal) { return incident - T{ 2 } *Dot(incident, normal) * normal; }

	template<typename T> inline Vec<T, 3> Refract(const Vec<T, 3>& incident, const Vec<T, 3>& normal, T eta) {
		T dot_ni = Dot(normal, incident);
		T k = T{ 1 } - eta * eta * (T{ 1 } - dot_ni * dot_ni);
		if (k < T{ 0 })
			return Vec<T, 3>{};
		return eta * incident - (eta * dot_ni + Sqrt(k)) * normal;
	}

	template<typename T> inline Vec<T, 3> Project(const Vec<T, 3>& vector, const Vec<T, 3>& normal) { return normal * (Dot(vector, normal) / Dot(normal, normal)); }

	template<typename T> inline Vec<T, 3> Reject(const Vec<T, 3>& vector, const Vec<T, 3>& normal) { return vector - Project(vector, normal); }

	template<typename T> inline T Angle(const Vec<T, 3>& a, const Vec<T, 3>& b) { return Acos(Dot(Normalize(a), Normalize(b))); }

	template<typename T> inline T SignedAngle(const Vec<T, 3>& a, const Vec<T, 3>& b, const Vec<T, 3>& ref) {
		T         angle = Angle(a, b);
		Vec<T, 3> cross_product = Cross(a, b);
		return angle * Sign(Dot(cross_product, ref));
	}

	template<typename T> inline T CopySign(T mag, T sign) { return std::copysign(mag, sign); }

	// Extended quaternion operations
	template<typename T> inline Quaternion<T> RotationBetweenVectors(const Vec<T, 3>& start, const Vec<T, 3>& end) {
		Vec<T, 3> start_norm = Normalize(start);
		Vec<T, 3> end_norm = Normalize(end);

		T         cos_theta = Dot(start_norm, end_norm);
		Vec<T, 3> rotation_axis;

		if (cos_theta < T{ -0.999999 }) {
			rotation_axis = Cross(Vec<T, 3>(0, 0, 1), start_norm);
			if (LengthSquared(rotation_axis) < T{ 0.000001 })
				rotation_axis = Cross(Vec<T, 3>(1, 0, 0), start_norm);
			rotation_axis = Normalize(rotation_axis);
			return Quaternion<T>(rotation_axis, T{ k_pi });
		}

		rotation_axis = Cross(start_norm, end_norm);
		T s = Sqrt((T{ 1 } + cos_theta) * T { 2 });
		T invs = T{ 1 } / s;

		return Quaternion<T>(rotation_axis.x * invs, rotation_axis.y * invs, rotation_axis.z * invs, s * T{ 0.5 });
	}

	template<typename T> inline Quaternion<T> LookAt(const Vec<T, 3>& direction, const Vec<T, 3>& up = Vec<T, 3>(0, 1, 0)) {
		Vec<T, 3> forward = Normalize(direction);
		Vec<T, 3> right = Normalize(Cross(up, forward));
		Vec<T, 3> up_actual = Cross(forward, right);

		Mat<T, 3, 3> rot;
		rot(0, 0) = right.x;
		rot(0, 1) = right.y;
		rot(0, 2) = right.z;
		rot(1, 0) = up_actual.x;
		rot(1, 1) = up_actual.y;
		rot(1, 2) = up_actual.z;
		rot(2, 0) = forward.x;
		rot(2, 1) = forward.y;
		rot(2, 2) = forward.z;

		return QuatCast(rot);
	}

	// Extended matrix operations
	template<typename T> inline T Determinant(const Mat<T, 2, 2>& m) { return m(0, 0) * m(1, 1) - m(0, 1) * m(1, 0); }

	template<typename T> inline T Determinant(const Mat<T, 3, 3>& m) {
		return m(0, 0) * (m(1, 1) * m(2, 2) - m(1, 2) * m(2, 1)) - m(0, 1) * (m(1, 0) * m(2, 2) - m(1, 2) * m(2, 0)) + m(0, 2) * (m(1, 0) * m(2, 1) - m(1, 1) * m(2, 0));
	}

	template<typename T> inline T Determinant(const Mat<T, 4, 4>& m) {
		return m(0, 3) * m(1, 2) * m(2, 1) * m(3, 0) - m(0, 2) * m(1, 3) * m(2, 1) * m(3, 0) - m(0, 3) * m(1, 1) * m(2, 2) * m(3, 0) + m(0, 1) * m(1, 3) * m(2, 2) * m(3, 0) +
			m(0, 2) * m(1, 1) * m(2, 3) * m(3, 0) - m(0, 1) * m(1, 2) * m(2, 3) * m(3, 0) - m(0, 3) * m(1, 2) * m(2, 0) * m(3, 1) + m(0, 2) * m(1, 3) * m(2, 0) * m(3, 1) +
			m(0, 3) * m(1, 0) * m(2, 2) * m(3, 1) - m(0, 0) * m(1, 3) * m(2, 2) * m(3, 1) - m(0, 2) * m(1, 0) * m(2, 3) * m(3, 1) + m(0, 0) * m(1, 2) * m(2, 3) * m(3, 1) +
			m(0, 3) * m(1, 1) * m(2, 0) * m(3, 2) - m(0, 1) * m(1, 3) * m(2, 0) * m(3, 2) - m(0, 3) * m(1, 0) * m(2, 1) * m(3, 2) + m(0, 0) * m(1, 3) * m(2, 1) * m(3, 2) +
			m(0, 1) * m(1, 0) * m(2, 3) * m(3, 2) - m(0, 0) * m(1, 1) * m(2, 3) * m(3, 2) - m(0, 2) * m(1, 1) * m(2, 0) * m(3, 3) + m(0, 1) * m(1, 2) * m(2, 0) * m(3, 3) +
			m(0, 2) * m(1, 0) * m(2, 1) * m(3, 3) - m(0, 0) * m(1, 2) * m(2, 1) * m(3, 3) - m(0, 1) * m(1, 0) * m(2, 2) * m(3, 3) + m(0, 0) * m(1, 1) * m(2, 2) * m(3, 3);
	}

	template<typename T> inline Mat<T, 4, 4> Inverse(const Mat<T, 4, 4>& m) {
		T det = Determinant(m);
		if (Abs(det) < k_epsilon) {
			return Mat<T, 4, 4>::Identity(); // Return identity if matrix is singular
		}

		Mat<T, 4, 4> adj;
		T            inv_det = T{ 1 } / det;

		// First row
		adj(0, 0) = (m(1, 1) * m(2, 2) * m(3, 3) + m(1, 2) * m(2, 3) * m(3, 1) + m(1, 3) * m(2, 1) * m(3, 2) - m(1, 1) * m(2, 3) * m(3, 2) - m(1, 2) * m(2, 1) * m(3, 3) - m(1, 3) * m(2, 2) * m(3, 1)) * inv_det;
		adj(0, 1) = (m(0, 1) * m(2, 3) * m(3, 2) + m(0, 2) * m(2, 1) * m(3, 3) + m(0, 3) * m(2, 2) * m(3, 1) - m(0, 1) * m(2, 2) * m(3, 3) - m(0, 2) * m(2, 3) * m(3, 1) - m(0, 3) * m(2, 1) * m(3, 2)) * inv_det;
		adj(0, 2) = (m(0, 1) * m(1, 2) * m(3, 3) + m(0, 2) * m(1, 3) * m(3, 1) + m(0, 3) * m(1, 1) * m(3, 2) - m(0, 1) * m(1, 3) * m(3, 2) - m(0, 2) * m(1, 1) * m(3, 3) - m(0, 3) * m(1, 2) * m(3, 1)) * inv_det;
		adj(0, 3) = (m(0, 1) * m(1, 3) * m(2, 2) + m(0, 2) * m(1, 1) * m(2, 3) + m(0, 3) * m(1, 2) * m(2, 1) - m(0, 1) * m(1, 2) * m(2, 3) - m(0, 2) * m(1, 3) * m(2, 1) - m(0, 3) * m(1, 1) * m(2, 2)) * inv_det;

		// Second row
		adj(1, 0) = (m(1, 0) * m(2, 3) * m(3, 2) + m(1, 2) * m(2, 0) * m(3, 3) + m(1, 3) * m(2, 2) * m(3, 0) - m(1, 0) * m(2, 2) * m(3, 3) - m(1, 2) * m(2, 3) * m(3, 0) - m(1, 3) * m(2, 0) * m(3, 2)) * inv_det;
		adj(1, 1) = (m(0, 0) * m(2, 2) * m(3, 3) + m(0, 2) * m(2, 3) * m(3, 0) + m(0, 3) * m(2, 0) * m(3, 2) - m(0, 0) * m(2, 3) * m(3, 2) - m(0, 2) * m(2, 0) * m(3, 3) - m(0, 3) * m(2, 2) * m(3, 0)) * inv_det;
		adj(1, 2) = (m(0, 0) * m(1, 3) * m(3, 2) + m(0, 2) * m(1, 0) * m(3, 3) + m(0, 3) * m(1, 2) * m(3, 0) - m(0, 0) * m(1, 2) * m(3, 3) - m(0, 2) * m(1, 3) * m(3, 0) - m(0, 3) * m(1, 0) * m(3, 2)) * inv_det;
		adj(1, 3) = (m(0, 0) * m(1, 2) * m(2, 3) + m(0, 2) * m(1, 3) * m(2, 0) + m(0, 3) * m(1, 0) * m(2, 2) - m(0, 0) * m(1, 3) * m(2, 2) - m(0, 2) * m(1, 0) * m(2, 3) - m(0, 3) * m(1, 2) * m(2, 0)) * inv_det;

		// Third row
		adj(2, 0) = (m(1, 0) * m(2, 1) * m(3, 3) + m(1, 1) * m(2, 3) * m(3, 0) + m(1, 3) * m(2, 0) * m(3, 1) - m(1, 0) * m(2, 3) * m(3, 1) - m(1, 1) * m(2, 0) * m(3, 3) - m(1, 3) * m(2, 1) * m(3, 0)) * inv_det;
		adj(2, 1) = (m(0, 0) * m(2, 3) * m(3, 1) + m(0, 1) * m(2, 0) * m(3, 3) + m(0, 3) * m(2, 1) * m(3, 0) - m(0, 0) * m(2, 1) * m(3, 3) - m(0, 1) * m(2, 3) * m(3, 0) - m(0, 3) * m(2, 0) * m(3, 1)) * inv_det;
		adj(2, 2) = (m(0, 0) * m(1, 1) * m(3, 3) + m(0, 1) * m(1, 3) * m(3, 0) + m(0, 3) * m(1, 0) * m(3, 1) - m(0, 0) * m(1, 3) * m(3, 1) - m(0, 1) * m(1, 0) * m(3, 3) - m(0, 3) * m(1, 1) * m(3, 0)) * inv_det;
		adj(2, 3) = (m(0, 0) * m(1, 3) * m(2, 1) + m(0, 1) * m(1, 0) * m(2, 3) + m(0, 3) * m(1, 1) * m(2, 0) - m(0, 0) * m(1, 1) * m(2, 3) - m(0, 1) * m(1, 3) * m(2, 0) - m(0, 3) * m(1, 0) * m(2, 1)) * inv_det;

		// Fourth row
		adj(3, 0) = (m(1, 0) * m(2, 2) * m(3, 1) + m(1, 1) * m(2, 0) * m(3, 2) + m(1, 2) * m(2, 1) * m(3, 0) - m(1, 0) * m(2, 1) * m(3, 2) - m(1, 1) * m(2, 2) * m(3, 0) - m(1, 2) * m(2, 0) * m(3, 1)) * inv_det;
		adj(3, 1) = (m(0, 0) * m(2, 1) * m(3, 2) + m(0, 1) * m(2, 2) * m(3, 0) + m(0, 2) * m(2, 0) * m(3, 1) - m(0, 0) * m(2, 2) * m(3, 1) - m(0, 1) * m(2, 0) * m(3, 2) - m(0, 2) * m(2, 1) * m(3, 0)) * inv_det;
		adj(3, 2) = (m(0, 0) * m(1, 2) * m(3, 1) + m(0, 1) * m(1, 0) * m(3, 2) + m(0, 2) * m(1, 1) * m(3, 0) - m(0, 0) * m(1, 1) * m(3, 2) - m(0, 1) * m(1, 2) * m(3, 0) - m(0, 2) * m(1, 0) * m(3, 1)) * inv_det;
		adj(3, 3) = (m(0, 0) * m(1, 1) * m(2, 2) + m(0, 1) * m(1, 2) * m(2, 0) + m(0, 2) * m(1, 0) * m(2, 1) - m(0, 0) * m(1, 2) * m(2, 1) - m(0, 1) * m(1, 0) * m(2, 2) - m(0, 2) * m(1, 1) * m(2, 0)) * inv_det;

		return adj;
	}

	// Transform decomposition
	template<typename T> inline void DecomposeTransform(const Mat<T, 4, 4>& transform, Vec<T, 3>& translation, Quaternion<T>& rotation, Vec<T, 3>& scale) {
		// Extract translation
		translation = Vec<T, 3>(transform(3, 0), transform(3, 1), transform(3, 2));

		// Extract scale
		Vec<T, 3> x_axis(transform(0, 0), transform(0, 1), transform(0, 2));
		Vec<T, 3> y_axis(transform(1, 0), transform(1, 1), transform(1, 2));
		Vec<T, 3> z_axis(transform(2, 0), transform(2, 1), transform(2, 2));

		scale.x = Length(x_axis);
		scale.y = Length(y_axis);
		scale.z = Length(z_axis);

		// Extract rotation
		Mat<T, 3, 3> rot_mat;
		rot_mat(0, 0) = x_axis.x / scale.x;
		rot_mat(0, 1) = x_axis.y / scale.x;
		rot_mat(0, 2) = x_axis.z / scale.x;
		rot_mat(1, 0) = y_axis.x / scale.y;
		rot_mat(1, 1) = y_axis.y / scale.y;
		rot_mat(1, 2) = y_axis.z / scale.y;
		rot_mat(2, 0) = z_axis.x / scale.z;
		rot_mat(2, 1) = z_axis.y / scale.z;
		rot_mat(2, 2) = z_axis.z / scale.z;

		rotation = QuatCast(rot_mat);
	}

	// Geometric functions
	template<typename T> inline bool RayPlaneIntersection(const Vec<T, 3>& ray_origin, const Vec<T, 3>& ray_dir, const Vec<T, 3>& plane_normal, const Vec<T, 3>& plane_point, T& t) {
		T denom = Dot(plane_normal, ray_dir);
		if (Abs(denom) > k_epsilon) {
			Vec<T, 3> p0l0 = plane_point - ray_origin;
			t = Dot(p0l0, plane_normal) / denom;
			return t >= T{ 0 };
		}
		return false;
	}

	template<typename T> inline bool RaySphereIntersection(const Vec<T, 3>& ray_origin, const Vec<T, 3>& ray_dir, const Vec<T, 3>& sphere_center, T sphere_radius, T& t) {
		Vec<T, 3> oc = ray_origin - sphere_center;
		T         a = Dot(ray_dir, ray_dir);
		T         b = T{ 2 } *Dot(oc, ray_dir);
		T         c = Dot(oc, oc) - sphere_radius * sphere_radius;
		T         discriminant = b * b - T{ 4 } *a * c;

		if (discriminant < T{ 0 })
			return false;

		t = (-b - Sqrt(discriminant)) / (T{ 2 } *a);
		return t >= T{ 0 };
	}

	// Interpolation functions
	template<typename T> inline T SmoothStep(T edge0, T edge1, T x) {
		x = Clamp((x - edge0) / (edge1 - edge0), T{ 0 }, T{ 1 });
		return x * x * (T{ 3 } - T{ 2 } *x);
	}

	template<typename T> inline Vec<T, 3> CatmullRom(const Vec<T, 3>& p0, const Vec<T, 3>& p1, const Vec<T, 3>& p2, const Vec<T, 3>& p3, T t) {
		T t2 = t * t;
		T t3 = t2 * t;

		return T{ 0.5 } *((T{ 2 } *p1) + (-p0 + p2) * t + (T{ 2 } *p0 - T{ 5 } *p1 + T{ 4 } *p2 - p3) * t2 + (-p0 + T{ 3 } *p1 - T{ 3 } *p2 + p3) * t3);
	}

	// Easing functions
	template<typename T> inline T EaseInQuad(T t) { return t * t; }

	template<typename T> inline T EaseOutQuad(T t) { return t * (T{ 2 } - t); }

	template<typename T> inline T EaseInOutQuad(T t) { return t < T{ 0.5 } ? T{ 2 } *t * t : -T{ 1 } + (T{ 4 } - T{ 2 } *t) * t; }

	namespace Detail {
		constexpr i32 k_perm[512] = { 151, 160, 137, 91,  90,  15,  131, 13,  201, 95,  96,  53,  194, 233, 7,   225, 140, 36,  103, 30,  69,  142, 8,   99,  37,  240, 21,  10,  23,  190, 6,   148,
									 247, 120, 234, 75,  0,   26,  197, 62,  94,  252, 219, 203, 117, 35,  11,  32,  57,  177, 33,  88,  237, 149, 56,  87,  174, 20,  125, 136, 171, 168, 68,  175,
									 74,  165, 71,  134, 139, 48,  27,  166, 77,  146, 158, 231, 83,  111, 229, 122, 60,  211, 133, 230, 220, 105, 92,  41,  55,  46,  245, 40,  244, 102, 143, 54,
									 65,  25,  63,  161, 1,   216, 80,  73,  209, 76,  132, 187, 208, 89,  18,  169, 200, 196, 135, 130, 116, 188, 159, 86,  164, 100, 109, 198, 173, 186, 3,   64,
									 52,  217, 226, 250, 124, 123, 5,   202, 38,  147, 118, 126, 255, 82,  85,  212, 207, 206, 59,  227, 47,  16,  58,  17,  182, 189, 28,  42,  223, 183, 170, 213,
									 119, 248, 152, 2,   44,  154, 163, 70,  221, 153, 101, 155, 167, 43,  172, 9,   129, 22,  39,  253, 19,  98,  108, 110, 79,  113, 224, 232, 178, 185, 112, 104,
									 218, 246, 97,  228, 251, 34,  242, 193, 238, 210, 144, 12,  191, 179, 162, 241, 81,  51,  145, 235, 249, 14,  239, 107, 49,  192, 214, 31,  181, 199, 106, 157,
									 184, 84,  204, 176, 115, 121, 50,  45,  127, 4,   150, 254, 138, 236, 205, 93,  222, 114, 67,  29,  24,  72,  243, 141, 128, 195, 78,  66,  215, 61,  156, 180 };

		inline f32 Fade(f32 t) { return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f); }

		inline f32 Grad(i32 hash, f32 x, f32 y, f32 z) {
			i32 h = hash & 15;
			f32 u = h < 8 ? x : y;
			f32 v = h < 4 ? y : h == 12 || h == 14 ? x : z;
			return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
		}

		inline i32 FastFloor(f32 x) { return x > 0 ? (i32)x : (i32)x - 1; }
	} // namespace Detail

	// Improved Perlin Noise implementation
	template<typename T> inline T PerlinNoise(const Vec<T, 3>& p) {
		using namespace Detail;

		i32 X = FastFloor(p.x) & 255;
		i32 Y = FastFloor(p.y) & 255;
		i32 Z = FastFloor(p.z) & 255;

		f32 x = p.x - FastFloor(p.x);
		f32 y = p.y - FastFloor(p.y);
		f32 z = p.z - FastFloor(p.z);

		f32 u = Fade(x);
		f32 v = Fade(y);
		f32 w = Fade(z);

		i32 A = k_perm[X] + Y;
		i32 AA = k_perm[A] + Z;
		i32 AB = k_perm[A + 1] + Z;
		i32 B = k_perm[X + 1] + Y;
		i32 BA = k_perm[B] + Z;
		i32 BB = k_perm[B + 1] + Z;

		return Lerp(Lerp(Lerp(Grad(k_perm[AA], x, y, z), Grad(k_perm[BA], x - 1, y, z), u), Lerp(Grad(k_perm[AB], x, y - 1, z), Grad(k_perm[BB], x - 1, y - 1, z), u), v),
			Lerp(Lerp(Grad(k_perm[AA + 1], x, y, z - 1), Grad(k_perm[BA + 1], x - 1, y, z - 1), u), Lerp(Grad(k_perm[AB + 1], x, y - 1, z - 1), Grad(k_perm[BB + 1], x - 1, y - 1, z - 1), u), v),
			w);
	}

	// Simplex Noise implementation
	template<typename T> inline T SimplexNoise(const Vec<T, 3>& p) {
		// Noise constants
		constexpr T F3 = T{ 1 } / T{ 3 };
		constexpr T G3 = T{ 1 } / T{ 6 };

		// Skew input space
		T         s = (p.x + p.y + p.z) * F3;
		Vec<T, 3> skewed = p + s;

		// Floor to get simplex cell origin
		Vec<T, 3> cell(Floor(skewed.x), Floor(skewed.y), Floor(skewed.z));

		// Unskew cell origin back to (x,y,z) space
		T         t = (cell.x + cell.y + cell.z) * G3;
		Vec<T, 3> unskewed = cell - t;

		// Relative coordinates with respect to simplex cell origin
		Vec<T, 3> diff = p - unskewed;

		// For 3D simplex noise, we need to determine which of the tetrahedron cells we're in
		i32 i1, j1, k1;
		i32 i2, j2, k2;

		if (diff.x >= diff.y) {
			if (diff.y >= diff.z) {
				i1 = 1;
				j1 = 0;
				k1 = 0;
				i2 = 1;
				j2 = 1;
				k2 = 0; // X Y Z order
			}
			else if (diff.x >= diff.z) {
				i1 = 1;
				j1 = 0;
				k1 = 0;
				i2 = 1;
				j2 = 0;
				k2 = 1; // X Z Y order
			}
			else {
				i1 = 0;
				j1 = 0;
				k1 = 1;
				i2 = 1;
				j2 = 0;
				k2 = 1; // Z X Y order
			}
		}
		else {
			if (diff.y < diff.z) {
				i1 = 0;
				j1 = 0;
				k1 = 1;
				i2 = 0;
				j2 = 1;
				k2 = 1; // Z Y X order
			}
			else if (diff.x < diff.z) {
				i1 = 0;
				j1 = 1;
				k1 = 0;
				i2 = 0;
				j2 = 1;
				k2 = 1; // Y Z X order
			}
			else {
				i1 = 0;
				j1 = 1;
				k1 = 0;
				i2 = 1;
				j2 = 1;
				k2 = 0; // Y X Z order
			}
		}

		// Calculate contributions from the five corners
		T n = T{ 0 };

		T t0 = T{ 0.5 } - diff.x * diff.x - diff.y * diff.y - diff.z * diff.z;
		if (t0 > T{ 0 }) {
			t0 *= t0;
			n += t0 * t0 * Detail::Grad(Detail::k_perm[Detail::k_perm[Detail::k_perm[i32(cell.x) & 255] + i32(cell.y) & 255] + i32(cell.z) & 255], diff.x, diff.y, diff.z);
		}

		Vec<T, 3> diff1 = diff - Vec<T, 3>(i1, j1, k1) + G3;
		T         t1 = T{ 0.5 } - diff1.x * diff1.x - diff1.y * diff1.y - diff1.z * diff1.z;
		if (t1 > T{ 0 }) {
			t1 *= t1;
			n += t1 * t1 * Detail::Grad(Detail::k_perm[Detail::k_perm[Detail::k_perm[i32(cell.x + i1) & 255] + i32(cell.y + j1) & 255] + i32(cell.z + k1) & 255], diff1.x, diff1.y, diff1.z);
		}

		Vec<T, 3> diff2 = diff - Vec<T, 3>(i2, j2, k2) + T{ 2 } *G3;
		T         t2 = T{ 0.5 } - diff2.x * diff2.x - diff2.y * diff2.y - diff2.z * diff2.z;
		if (t2 > T{ 0 }) {
			t2 *= t2;
			n += t2 * t2 * Detail::Grad(Detail::k_perm[Detail::k_perm[Detail::k_perm[i32(cell.x + i2) & 255] + i32(cell.y + j2) & 255] + i32(cell.z + k2) & 255], diff2.x, diff2.y, diff2.z);
		}

		Vec<T, 3> diff3 = diff - Vec<T, 3>(1, 1, 1) + T{ 3 } *G3;
		T         t3 = T{ 0.5 } - diff3.x * diff3.x - diff3.y * diff3.y - diff3.z * diff3.z;
		if (t3 > T{ 0 }) {
			t3 *= t3;
			n += t3 * t3 * Detail::Grad(Detail::k_perm[Detail::k_perm[Detail::k_perm[i32(cell.x + 1) & 255] + i32(cell.y + 1) & 255] + i32(cell.z + 1) & 255], diff3.x, diff3.y, diff3.z);
		}

		// Scale to stay within [-1,1]
		return T{ 32 } *n;
	}

	// Voronoi Noise
	template<typename T> inline T VoronoiNoise(const Vec<T, 3>& p) {
		Vec<T, 3> cell_pos(Floor(p.x), Floor(p.y), Floor(p.z));

		T min_dist = T{ 1000 };

		for (i32 z = -1; z <= 1; z++) {
			for (i32 y = -1; y <= 1; y++) {
				for (i32 x = -1; x <= 1; x++) {
					Vec<T, 3> neighbor = cell_pos + Vec<T, 3>(x, y, z);
					Vec<T, 3> rand_point(neighbor.x + Random(), neighbor.y + Random(), neighbor.z + Random());
					T         dist = Distance(p, rand_point);
					min_dist = Min(min_dist, dist);
				}
			}
		}

		return min_dist;
	}

	// Additional utility functions
	template<typename T> inline Vec<T, 3> Orthogonal(const Vec<T, 3>& v) {
		T         ax = Abs(v.x);
		T         ay = Abs(v.y);
		T         az = Abs(v.z);
		Vec<T, 3> other = ax < ay ? (ax < az ? Vec<T, 3>(1, 0, 0) : Vec<T, 3>(0, 0, 1)) : (ay < az ? Vec<T, 3>(0, 1, 0) : Vec<T, 3>(0, 0, 1));
		return Normalize(Cross(v, other));
	}

	template<typename T> inline Mat<T, 3, 3> OuterProduct(const Vec<T, 3>& a, const Vec<T, 3>& b) {
		Mat<T, 3, 3> result;
		for (usize i = 0; i < 3; ++i)
			for (usize j = 0; j < 3; ++j)
				result(i, j) = a[i] * b[j];
		return result;
	}

	template<typename T> inline T OrientedAngle(const Vec<T, 2>& a, const Vec<T, 2>& b) {
		T angle = Atan2(b.y, b.x) - Atan2(a.y, a.x);
		if (angle > k_pi)
			angle -= T{ k_two_pi };
		if (angle <= -k_pi)
			angle += T{ k_two_pi };
		return angle;
	}

	template<typename T> inline Vec<T, 3> SphericalToCartesian(T radius, T theta, T phi) {
		return Vec<T, 3>(radius * Sin(theta) * Cos(phi), radius * Sin(theta) * Sin(phi), radius * Cos(theta));
	}

	template<typename T> inline void CartesianToSpherical(const Vec<T, 3>& cart, T& radius, T& theta, T& phi) {
		radius = Length(cart);
		theta = Acos(cart.z / radius);
		phi = Atan2(cart.y, cart.x);
	}

	template<typename T> inline T FractalNoise(const Vec<T, 3>& p, i32 octaves, T frequency = T{ 1 }, T persistence = T{ 0.5 }) {
		T total = T{ 0 };
		T amplitude = T{ 1 };
		T max_value = T{ 0 };

		for (i32 i = 0; i < octaves; ++i) {
			total += PerlinNoise(p * frequency) * amplitude;
			max_value += amplitude;
			frequency *= T{ 2 };
			amplitude *= persistence;
		}

		return total / max_value;
	}

	// Color space conversions
	template<typename T> inline Vec<T, 3> RGBtoHSV(const Vec<T, 3>& rgb) {
		T r = rgb.R(), g = rgb.G(), b = rgb.B();
		T max_val = Max(Max(r, g), b);
		T min_val = Min(Min(r, g), b);
		T diff = max_val - min_val;

		Vec<T, 3> hsv;
		hsv.x = T{ 0 };                                      // Hue
		hsv.y = (max_val == T{ 0 } ? T{ 0 } : diff / max_val); // Saturation
		hsv.z = max_val;                                   // Value

		if (diff != T{ 0 }) {
			if (max_val == r) {
				hsv.x = T{ 60 } *((g - b) / diff);
			}
			else if (max_val == g) {
				hsv.x = T{ 60 } *(T{ 2 } + (b - r) / diff);
			}
			else {
				hsv.x = T{ 60 } *(T{ 4 } + (r - g) / diff);
			}
		}

		if (hsv.x < T{ 0 })
			hsv.x += T{ 360 };
		return hsv;
	}

	template<typename T> inline Vec<T, 3> HSVtoRGB(const Vec<T, 3>& hsv) {
		T         h = hsv.x, s = hsv.y, v = hsv.z;
		Vec<T, 3> rgb;

		if (s == T{ 0 }) {
			rgb = Vec<T, 3>(v, v, v);
		}
		else {
			T   sector = h / T{ 60 };
			i32 i = static_cast<i32>(sector);
			T   f = sector - static_cast<T>(i);
			T   p = v * (T{ 1 } - s);
			T   q = v * (T{ 1 } - s * f);
			T   t = v * (T{ 1 } - s * (T{ 1 } - f));

			switch (i % 6) {
			case 0:
				rgb = Vec<T, 3>(v, t, p);
				break;
			case 1:
				rgb = Vec<T, 3>(q, v, p);
				break;
			case 2:
				rgb = Vec<T, 3>(p, v, t);
				break;
			case 3:
				rgb = Vec<T, 3>(p, q, v);
				break;
			case 4:
				rgb = Vec<T, 3>(t, p, v);
				break;
			case 5:
				rgb = Vec<T, 3>(v, p, q);
				break;
			}
		}
		return rgb;
	}

	template<typename T> inline Vec<T, 3> RotateAroundAxis(const Vec<T, 3>& v, const Vec<T, 3>& axis, T angle) {
		T c = Cos(angle);
		T s = Sin(angle);
		T k = T{ 1 } - c;

		Vec<T, 3> normalized_axis = Normalize(axis);
		T         x = normalized_axis.x;
		T         y = normalized_axis.y;
		T         z = normalized_axis.z;

		return Vec<T, 3>((k * x * x + c) * v.x + (k * x * y - s * z) * v.y + (k * x * z + s * y) * v.z,
			(k * x * y + s * z) * v.x + (k * y * y + c) * v.y + (k * y * z - s * x) * v.z,
			(k * x * z - s * y) * v.x + (k * y * z + s * x) * v.y + (k * z * z + c) * v.z);
	}

	// Matrix generators
	template<typename T> inline Mat<T, 4, 4> CreatePerspectiveRH(T fovy, T aspect, T near_plane, T far_plane) {
		T f = T{ 1 } / Tan(fovy / T{ 2 });

		Mat<T, 4, 4> result;
		result(0, 0) = f / aspect;
		result(1, 1) = f;
		result(2, 2) = (far_plane + near_plane) / (near_plane - far_plane);
		result(2, 3) = -T{ 1 };
		result(3, 2) = (T{ 2 } *far_plane * near_plane) / (near_plane - far_plane);
		return result;
	}

	template<typename T> inline Mat<T, 4, 4> CreatePerspectiveLH(T fovy, T aspect, T near_plane, T far_plane) {
		T f = T{ 1 } / Tan(fovy / T{ 2 });

		Mat<T, 4, 4> result;
		result(0, 0) = f / aspect;
		result(1, 1) = f;
		result(2, 2) = (far_plane + near_plane) / (far_plane - near_plane);
		result(2, 3) = T{ 1 };
		result(3, 2) = (-T{ 2 } *far_plane * near_plane) / (far_plane - near_plane);
		return result;
	}

	template<typename T> inline Mat<T, 4, 4> CreateOrthographicRH(T left, T right, T bottom, T top, T near_plane, T far_plane) {
		Mat<T, 4, 4> result;
		result(0, 0) = T{ 2 } / (right - left);
		result(1, 1) = T{ 2 } / (top - bottom);
		result(2, 2) = T{ 2 } / (near_plane - far_plane);
		result(3, 0) = (left + right) / (left - right);
		result(3, 1) = (top + bottom) / (bottom - top);
		result(3, 2) = (far_plane + near_plane) / (near_plane - far_plane);
		result(3, 3) = T{ 1 };
		return result;
	}

	// Bezier curves
	template<typename T> inline Vec<T, 3> QuadraticBezier(const Vec<T, 3>& p0, const Vec<T, 3>& p1, const Vec<T, 3>& p2, T t) {
		T one_minus_t = T{ 1 } - t;
		return one_minus_t * one_minus_t * p0 + T{ 2 } *one_minus_t * t * p1 + t * t * p2;
	}

	template<typename T> inline Vec<T, 3> CubicBezier(const Vec<T, 3>& p0, const Vec<T, 3>& p1, const Vec<T, 3>& p2, const Vec<T, 3>& p3, T t) {
		T one_minus_t = T{ 1 } - t;
		return one_minus_t * one_minus_t * one_minus_t * p0 + T{ 3 } *one_minus_t * one_minus_t * t * p1 + T{ 3 } *one_minus_t * t * t * p2 + t * t * t * p3;
	}

	// Spline i32erpolation
	template<typename T> inline Vec<T, 3> CatmullRomSpline(const Vec<T, 3>& p0, const Vec<T, 3>& p1, const Vec<T, 3>& p2, const Vec<T, 3>& p3, T t, T tension = T{ 0.5 }) {
		T t2 = t * t;
		T t3 = t2 * t;

		Vec<T, 3> m0 = tension * (p2 - p0);
		Vec<T, 3> m1 = tension * (p3 - p1);

		return (T{ 2 } *t3 - T{ 3 } *t2 + T{ 1 }) * p1 + (t3 - T{ 2 } *t2 + t) * m0 + (-T{ 2 } *t3 + T{ 3 } *t2) * p2 + (t3 - t2) * m1;
	}

	// Advanced geometric functions
	template<typename T> inline bool RayTriangleIntersection(const Vec<T, 3>& ray_origin, const Vec<T, 3>& ray_dir, const Vec<T, 3>& v0, const Vec<T, 3>& v1, const Vec<T, 3>& v2, T& t, Vec<T, 2>& bary) {
		Vec<T, 3> edge1 = v1 - v0;
		Vec<T, 3> edge2 = v2 - v0;
		Vec<T, 3> h = Cross(ray_dir, edge2);
		T         a = Dot(edge1, h);

		if (a > -k_epsilon && a < k_epsilon)
			return false;

		T         f = T{ 1 } / a;
		Vec<T, 3> s = ray_origin - v0;
		bary.x = f * Dot(s, h);

		if (bary.x < T{ 0 } || bary.x > T{ 1 })
			return false;

		Vec<T, 3> q = Cross(s, edge1);
		bary.y = f * Dot(ray_dir, q);

		if (bary.y < T{ 0 } || bary.x + bary.y > T{ 1 })
			return false;

		t = f * Dot(edge2, q);
		return t > k_epsilon;
	}

	// Frustum functions
	template<typename T> class Frustum {
	public:
		enum Planes { LEFT = 0, RIGHT, BOTTOM, TOP, NEAR, FAR, COUNT };

		void ExtractFromViewProjection(const Mat<T, 4, 4>& view_projection) {
			for (size_t i = 0; i < 4; ++i) {
				m_planes[LEFT][i] = view_projection(i, 3) + view_projection(i, 0);
				m_planes[RIGHT][i] = view_projection(i, 3) - view_projection(i, 0);
				m_planes[BOTTOM][i] = view_projection(i, 3) + view_projection(i, 1);
				m_planes[TOP][i] = view_projection(i, 3) - view_projection(i, 1);
				m_planes[NEAR][i] = view_projection(i, 3) + view_projection(i, 2);
				m_planes[FAR][i] = view_projection(i, 3) - view_projection(i, 2);
			}
			for (size_t i = 0; i < COUNT; ++i) {
				T length = Length(Vec<T, 3>(m_planes[i].x, m_planes[i].y, m_planes[i].z));
				m_planes[i] = m_planes[i] / length;
			}
		}

		bool PointInFrustum(const Vec<T, 3>& point) const {
			for (i32 i = 0; i < COUNT; ++i) {
				if (Dot(Vec<T, 3>(m_planes[i].x, m_planes[i].y, m_planes[i].z), point) + m_planes[i].w < T{ 0 })
					return false;
			}
			return true;
		}

		bool SphereInFrustum(const Vec<T, 3>& center, T radius) const {
			for (i32 i = 0; i < COUNT; ++i) {
				if (Dot(Vec<T, 3>(m_planes[i].x, m_planes[i].y, m_planes[i].z), center) + m_planes[i].w < -radius)
					return false;
			}
			return true;
		}

	private:
		std::array<Vec<T, 4>, COUNT> m_planes;
	};

	template<typename T> struct AABB {
		Vec<T, 3> min_point;
		Vec<T, 3> max_point;

		AABB()
			: min_point(T{ 0 })
			, max_point(T{ 0 }) {}
		AABB(const Vec<T, 3>& min, const Vec<T, 3>& max)
			: min_point(min)
			, max_point(max) {}

		bool Intersects(const AABB& other) const {
			return (min_point.x <= other.max_point.x && max_point.x >= other.min_point.x) && (min_point.y <= other.max_point.y && max_point.y >= other.min_point.y) &&
				(min_point.z <= other.max_point.z && max_point.z >= other.min_point.z);
		}

		bool Contains(const Vec<T, 3>& point) const {
			return point.x >= min_point.x && point.x <= max_point.x && point.y >= min_point.y && point.y <= max_point.y && point.z >= min_point.z && point.z <= max_point.z;
		}

		Vec<T, 3> GetCenter() const { return (min_point + max_point) * T { 0.5 }; }

		Vec<T, 3> GetExtents() const { return (max_point - min_point) * T { 0.5 }; }
	};

	// OBB (Oriented Bounding Box) class and i32ersection tests
	template<typename T> class OBB {
	public:
		Vec<T, 3>    center;
		Vec<T, 3>    extents;
		Mat<T, 3, 3> orientation;

		OBB()
			: center(T{ 0 })
			, extents(T{ 1 })
			, orientation(Mat<T, 3, 3>::Identity()) {}

		bool Intersects(const OBB& other) const {
			// Separating Axis Theorem (SAT) implementation
			const i32 NUM_AXES = 15;
			Vec<T, 3> axes[NUM_AXES];

			// Get box axes
			for (i32 i = 0; i < 3; ++i) {
				axes[i] = Vec<T, 3>(orientation(0, i), orientation(1, i), orientation(2, i));
				axes[i + 3] = Vec<T, 3>(other.orientation(0, i), other.orientation(1, i), other.orientation(2, i));
			}

			// Cross products of axes
			i32 curr = 6;
			for (i32 i = 0; i < 3; ++i) {
				for (i32 j = 0; j < 3; ++j) {
					axes[curr++] = Cross(axes[i], axes[j + 3]);
				}
			}

			// Test all axes
			for (i32 i = 0; i < NUM_AXES; ++i) {
				if (Length(axes[i]) < k_epsilon)
					continue;

				Vec<T, 3> axis = Normalize(axes[i]);
				T         r1 = ProjectionRadius(*this, axis);
				T         r2 = ProjectionRadius(other, axis);

				Vec<T, 3> d = other.center - center;
				T         dist = Abs(Dot(d, axis));

				if (dist > r1 + r2)
					return false;
			}

			return true;
		}

	private:
		static T ProjectionRadius(const OBB<T>& obb, const Vec<T, 3>& axis) {
			return obb.extents.x * Abs(Dot(axis, Vec<T, 3>(obb.orientation(0, 0), obb.orientation(1, 0), obb.orientation(2, 0)))) +
				obb.extents.y * Abs(Dot(axis, Vec<T, 3>(obb.orientation(0, 1), obb.orientation(1, 1), obb.orientation(2, 1)))) +
				obb.extents.z * Abs(Dot(axis, Vec<T, 3>(obb.orientation(0, 2), obb.orientation(1, 2), obb.orientation(2, 2))));
		}
	};

	inline Mat4 Orthographic(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far)
	{
		Mat4 result(1.0f); // Assume this creates an identity matrix.
		result(0, 0) = 2.0f / (right - left);
		result(1, 1) = 2.0f / (top - bottom);
		result(2, 2) = -2.0f / (far - near);
		result(3, 0) = -(right + left) / (right - left);
		result(3, 1) = -(top + bottom) / (top - bottom);
		result(3, 2) = -(far + near) / (far - near);
		return result;
	}

	// Additional Quaternion utilities
	template<typename T> inline Quaternion<T> QuatFromAxisAngle(const Vec<T, 3>& axis, T angle) {
		T half_angle = angle * T{ 0.5 };
		T sin_half = Sin(half_angle);
		return Quaternion<T>(axis.x * sin_half, axis.y * sin_half, axis.z * sin_half, Cos(half_angle));
	}

	template<typename T> inline void QuatToAxisAngle(const Quaternion<T>& q, Vec<T, 3>& axis, T& angle) {
		T sin_half_angle = Sqrt(T{ 1 } - q.w * q.w);
		if (sin_half_angle < k_epsilon) {
			axis = Vec<T, 3>(T{ 1 }, T{ 0 }, T{ 0 });
			angle = T{ 0 };
		}
		else {
			axis = Vec<T, 3>(q.x, q.y, q.z) / sin_half_angle;
			angle = T{ 2 } *Acos(q.w);
		}
	}

	// Matrix conversion utilities
	template<typename T> inline Mat<T, 3, 3> QuatToMat3(const Quaternion<T>& q) {
		Mat<T, 3, 3> result;
		T            xx = q.x * q.x, xy = q.x * q.y, xz = q.x * q.z, xw = q.x * q.w;
		T            yy = q.y * q.y, yz = q.y * q.z, yw = q.y * q.w;
		T            zz = q.z * q.z, zw = q.z * q.w;

		result(0, 0) = T{ 1 } - T{ 2 } *(yy + zz);
		result(0, 1) = T{ 2 } *(xy - zw);
		result(0, 2) = T{ 2 } *(xz + yw);
		result(1, 0) = T{ 2 } *(xy + zw);
		result(1, 1) = T{ 1 } - T{ 2 } *(xx + zz);
		result(1, 2) = T{ 2 } *(yz - xw);
		result(2, 0) = T{ 2 } *(xz - yw);
		result(2, 1) = T{ 2 } *(yz + xw);
		result(2, 2) = T{ 1 } - T{ 2 } *(xx + yy);

		return result;
	}

	template<typename T> inline Mat<T, 4, 4> QuatToMat4(const Quaternion<T>& q) {
		Mat<T, 4, 4> result;
		Mat<T, 3, 3> rot = QuatToMat3(q);

		for (usize i = 0; i < 3; ++i)
			for (usize j = 0; j < 3; ++j)
				result(i, j) = rot(i, j);

		result(3, 3) = T{ 1 };
		return result;
	}

	// Additional geometric utilities
	template<typename T> inline bool RayAABBIntersection(const Vec<T, 3>& ray_origin, const Vec<T, 3>& ray_dir, const AABB<T>& aabb, T& t_min, T& t_max) {
		Vec<T, 3> inv_dir(T{ 1 } / ray_dir.x, T{ 1 } / ray_dir.y, T{ 1 } / ray_dir.z);
		Vec<T, 3> t1 = (aabb.min_point - ray_origin) * inv_dir;
		Vec<T, 3> t2 = (aabb.max_point - ray_origin) * inv_dir;

		Vec<T, 3> t_mins(Min(t1.x, t2.x), Min(t1.y, t2.y), Min(t1.z, t2.z));
		Vec<T, 3> t_maxs(Max(t1.x, t2.x), Max(t1.y, t2.y), Max(t1.z, t2.z));

		t_min = Max(Max(t_mins.x, t_mins.y), t_mins.z);
		t_max = Min(Min(t_maxs.x, t_maxs.y), t_maxs.z);

		return t_max >= t_min && t_max >= T{ 0 };
	}

	template<typename T> inline bool RayOBBIntersection(const Vec<T, 3>& ray_origin, const Vec<T, 3>& ray_dir, const OBB<T>& obb, T& t) {
		// Transform ray to OBB space
		Vec<T, 3> p = ray_origin - obb.center;
		Vec<T, 3> local_dir(Dot(ray_dir, Vec<T, 3>(obb.orientation(0, 0), obb.orientation(1, 0), obb.orientation(2, 0))),
			Dot(ray_dir, Vec<T, 3>(obb.orientation(0, 1), obb.orientation(1, 1), obb.orientation(2, 1))),
			Dot(ray_dir, Vec<T, 3>(obb.orientation(0, 2), obb.orientation(1, 2), obb.orientation(2, 2))));
		Vec<T, 3> local_pos(Dot(p, Vec<T, 3>(obb.orientation(0, 0), obb.orientation(1, 0), obb.orientation(2, 0))),
			Dot(p, Vec<T, 3>(obb.orientation(0, 1), obb.orientation(1, 1), obb.orientation(2, 1))),
			Dot(p, Vec<T, 3>(obb.orientation(0, 2), obb.orientation(1, 2), obb.orientation(2, 2))));

		AABB<T> local_aabb(obb.extents * T{ -1 }, obb.extents);
		T       t_min, t_max;
		if (RayAABBIntersection(local_pos, local_dir, local_aabb, t_min, t_max)) {
			t = t_min;
			return true;
		}
		return false;
	}

	// Transform utilities
	template<typename T> inline Mat<T, 4, 4> CreateViewMatrix(const Vec<T, 3>& eye_pos, const Vec<T, 3>& target_pos, const Vec<T, 3>& up) {
		Vec<T, 3> z_axis = Normalize(eye_pos - target_pos);
		Vec<T, 3> x_axis = Normalize(Cross(up, z_axis));
		Vec<T, 3> y_axis = Cross(z_axis, x_axis);

		Mat<T, 4, 4> result = Mat<T, 4, 4>::Identity();
		result(0, 0) = x_axis.x;
		result(0, 1) = x_axis.y;
		result(0, 2) = x_axis.z;
		result(1, 0) = y_axis.x;
		result(1, 1) = y_axis.y;
		result(1, 2) = y_axis.z;
		result(2, 0) = z_axis.x;
		result(2, 1) = z_axis.y;
		result(2, 2) = z_axis.z;
		result(0, 3) = -Dot(x_axis, eye_pos);
		result(1, 3) = -Dot(y_axis, eye_pos);
		result(2, 3) = -Dot(z_axis, eye_pos);

		return result;
	}

	// Additional i32erpolation functions
	template<typename T> inline T BounceEase(T t) {
		T t2 = t * t;
		return t < T{ 0.5 } ? T{ 8 } *t2 * t2 : T{ 1 } - T{ 8 } *(t - T{ 1 }) * (t - T{ 1 }) * (t - T{ 1 }) * (t - T{ 1 });
	}

	template<typename T> inline T ElasticEase(T t) { return Sin(T{ 13 } *k_half_pi * t) * std::pow(T{ 2 }, T{ 10 } *(t - T{ 1 })); }

	template<typename T> inline T BackEase(T t) {
		T s = T{ 1.70158 };
		return t * t * ((s + T{ 1 }) * t - s);
	}

	// Additional noise functions
	template<typename T> inline T ValueNoise(const Vec<T, 3>& p) {
		Vec<T, 3> floor_p(Floor(p.x), Floor(p.y), Floor(p.z));
		Vec<T, 3> fract_p = p - floor_p;

		// Smooth i32erpolation
		Vec<T, 3> smooth_p(Fade(fract_p.x), Fade(fract_p.y), Fade(fract_p.z));

		// Generate random values at lattice points
		auto hash = [](i32 x, i32 y, i32 z) {
			i32 n = x + y * 57 + z * 113;
			n = (n << 13) ^ n;
			return (T{ 1 } - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / T{ 1073741824 });
			};

		return TrilinearInterpolation(hash(i32(floor_p.x), i32(floor_p.y), i32(floor_p.z)),
			hash(i32(floor_p.x + 1), i32(floor_p.y), i32(floor_p.z)),
			hash(i32(floor_p.x), i32(floor_p.y + 1), i32(floor_p.z)),
			hash(i32(floor_p.x + 1), i32(floor_p.y + 1), i32(floor_p.z)),
			hash(i32(floor_p.x), i32(floor_p.y), i32(floor_p.z + 1)),
			hash(i32(floor_p.x + 1), i32(floor_p.y), i32(floor_p.z + 1)),
			hash(i32(floor_p.x), i32(floor_p.y + 1), i32(floor_p.z + 1)),
			hash(i32(floor_p.x + 1), i32(floor_p.y + 1), i32(floor_p.z + 1)),
			smooth_p.x,
			smooth_p.y,
			smooth_p.z);
	}

	template<typename T> inline T TrilinearInterpolation(T c000, T c100, T c010, T c110, T c001, T c101, T c011, T c111, T x, T y, T z) {
		return Lerp(Lerp(Lerp(c000, c100, x), Lerp(c010, c110, x), y), Lerp(Lerp(c001, c101, x), Lerp(c011, c111, x), y), z);
	}

	// Euler angles to Quaternion conversions
	template<typename T> inline Quaternion<T> EulerToQuaternion(const Vec<T, 3>& euler) {
		T cx = Cos(euler.x * T { 0.5 });
		T cy = Cos(euler.y * T { 0.5 });
		T cz = Cos(euler.z * T { 0.5 });
		T sx = Sin(euler.x * T { 0.5 });
		T sy = Sin(euler.y * T { 0.5 });
		T sz = Sin(euler.z * T { 0.5 });

		return Quaternion<T>(sx * cy * cz - cx * sy * sz, // x
			cx * sy * cz + sx * cy * sz, // y
			cx * cy * sz - sx * sy * cz, // z
			cx * cy * cz + sx * sy * sz  // w
		);
	}

	// Overloads for different angle input types
	template<typename T> inline Quaternion<T> EulerXYZ(T x, T y, T z) { return EulerToQuaternion(Vec<T, 3>(x, y, z)); }

	template<typename T> inline Quaternion<T> EulerXYZ(const Vec<T, 3>& angles) { return EulerToQuaternion(angles); }

	// Quaternion to Euler angles conversion
	template<typename T> inline Vec<T, 3> QuaternionToEuler(const Quaternion<T>& q) {
		Vec<T, 3> euler;

		// Roll (x-axis rotation)
		T sinr_cosp = T{ 2 } *(q.w * q.x + q.y * q.z);
		T cosr_cosp = T{ 1 } - T{ 2 } *(q.x * q.x + q.y * q.y);
		euler.x = Atan2(sinr_cosp, cosr_cosp);

		// Pitch (y-axis rotation)
		T sinp = T{ 2 } *(q.w * q.y - q.z * q.x);
		if (Abs(sinp) >= T{ 1 })
			euler.y = CopySign(k_half_pi, sinp); // Use 90 degrees if out of range
		else
			euler.y = Asin(sinp);

		// Yaw (z-axis rotation)
		T siny_cosp = T{ 2 } *(q.w * q.z + q.x * q.y);
		T cosy_cosp = T{ 1 } - T{ 2 } *(q.y * q.y + q.z * q.z);
		euler.z = Atan2(siny_cosp, cosy_cosp);

		return euler;
	}

	// Helper function for sign copying

	template<typename T> inline Vec<T, 3> Translate(const Vec<T, 3>& v, const Vec<T, 3>& translation) { return v + translation; }

	template<typename T> inline Vec<T, 4> Translate(const Vec<T, 4>& v, const Vec<T, 3>& translation) {
		return Vec<T, 4>(v.x + translation.x * v.w, v.y + translation.y * v.w, v.z + translation.z * v.w, v.w);
	}

	template<typename T> inline Mat<T, 4, 4> Translate(const Mat<T, 4, 4>& m, const Vec<T, 3>& translation) {
		Mat<T, 4, 4> result = m;
		// Update the translation components in the fourth row
		for (usize i = 0; i < 4; ++i) {
			result(3, i) = m(0, i) * translation.x + m(1, i) * translation.y + m(2, i) * translation.z + m(3, i);
		}
		return result;
	}

	template<typename T> inline Mat<T, 4, 4> CreateTranslation(const Vec<T, 3>& translation) {
		Mat<T, 4, 4> result(T{ 1 });
		result(3, 0) = translation.x;
		result(3, 1) = translation.y;
		result(3, 2) = translation.z;
		return result;
	}

	template<typename T> inline Mat<T, 4, 4> Scale(const Vec<T, 3>& scale) {
		Mat<T, 4, 4> result = Mat<T, 4, 4>::Identity();
		result(0, 0) = scale.x;
		result(1, 1) = scale.y;
		result(2, 2) = scale.z;
		return result;
	}

	template<typename T> inline Mat<T, 4, 4> Perspective(T fovy, T aspect, T near_plane, T far_plane) {
		T            f = T{ 1 } / std::tan(fovy / T{ 2 });
		Mat<T, 4, 4> result = Mat<T, 4, 4>::Identity();
		result(0, 0) = f / aspect;
		result(1, 1) = f;
		result(2, 2) = (far_plane + near_plane) / (near_plane - far_plane);
		result(2, 3) = -T{ 1 };
		result(3, 2) = (T{ 2 } *far_plane * near_plane) / (near_plane - far_plane);
		return result;
	}

	// Apply scaling to an existing matrix
	template<typename T> inline Mat<T, 4, 4> Scale(const Mat<T, 4, 4>& matrix, const Vec<T, 3>& scale) {
		Mat<T, 4, 4> scale_matrix = Scale(scale);
		return matrix * scale_matrix;
	}

	template<typename T> inline T Radians(T degrees) { return degrees * (T{ k_pi } / T{ 180 }); }


	// Additional utility for translation composition
	template<typename T> inline Mat<T, 4, 4> TranslateX(const Mat<T, 4, 4>& m, T x) { return Translate(m, Vec<T, 3>(x, T{ 0 }, T{ 0 })); }

	template<typename T> inline Mat<T, 4, 4> TranslateY(const Mat<T, 4, 4>& m, T y) { return Translate(m, Vec<T, 3>(T{ 0 }, y, T{ 0 })); }

	template<typename T> inline Mat<T, 4, 4> TranslateZ(const Mat<T, 4, 4>& m, T z) { return Translate(m, Vec<T, 3>(T{ 0 }, T{ 0 }, z)); }
} // namespace spark::math

// Global operators
namespace spark {
	// Vector operators
	template<typename T, usize N> inline math::Vec<T, N> operator*(T scalar, const math::Vec<T, N>& vec) { return vec * scalar; }

	template<typename T, usize N> inline math::Vec<T, N> operator/(T scalar, const math::Vec<T, N>& vec) {
		math::Vec<T, N> result;
		for (usize i = 0; i < N; ++i) {
			if (vec[i] == T{})
				throw std::runtime_error("Division by zero");
			result[i] = scalar / vec[i];
		}
		return result;
	}

	// math::Matrix operators
	template<typename T, usize R, usize C> inline math::Mat<T, R, C> operator*(T scalar, const math::Mat<T, R, C>& mat) { return mat * scalar; }

	// math::Quaternion operators
	template<typename T> inline math::Quaternion<T> operator*(T scalar, const math::Quaternion<T>& quat) { return quat * scalar; }

	template<typename T, usize N> inline math::Vec<T, N> operator+(const math::Vec<T, N>& a, const math::Vec<T, N>& b) {
		math::Vec<T, N> result = a;
		result += b;
		return result;
	}

	template<typename T, usize N> inline math::Vec<T, N> operator-(const math::Vec<T, N>& a, const math::Vec<T, N>& b) {
		math::Vec<T, N> result = a;
		result -= b;
		return result;
	}

	template<typename T, usize N> inline math::Vec<T, N> operator*(const math::Vec<T, N>& v, T scalar) {
		math::Vec<T, N> result = v;
		result *= scalar;
		return result;
	}

	template<typename T, usize N> inline math::Vec<T, N> operator/(const math::Vec<T, N>& v, T scalar) {
		math::Vec<T, N> result = v;
		result /= scalar;
		return result;
	}

	// Global operators for math::Mat
	template<typename T, usize R, usize C> inline math::Mat<T, R, C> operator+(const math::Mat<T, R, C>& a, const math::Mat<T, R, C>& b) {
		math::Mat<T, R, C> result = a;
		result += b;
		return result;
	}

	template<typename T, usize R, usize C> inline math::Mat<T, R, C> operator-(const math::Mat<T, R, C>& a, const math::Mat<T, R, C>& b) {
		math::Mat<T, R, C> result = a;
		result -= b;
		return result;
	}

	template<typename T, usize R, usize C> inline math::Mat<T, R, C> operator*(const math::Mat<T, R, C>& m, T scalar) {
		math::Mat<T, R, C> result = m;
		result *= scalar;
		return result;
	}

	// Global operators for Quat
	template<typename T> inline math::Quaternion<T> operator+(const math::Quaternion<T>& a, const math::Quaternion<T>& b) {
		math::Quaternion<T> result = a;
		result += b;
		return result;
	}

	template<typename T> inline math::Quaternion<T> operator-(const math::Quaternion<T>& a, const math::Quaternion<T>& b) {
		math::Quaternion<T> result = a;
		result -= b;
		return result;
	}

	template<typename T> inline math::Quaternion<T> operator*(const math::Quaternion<T>& q, T scalar) {
		math::Quaternion<T> result = q;
		result *= scalar;
		return result;
	}

	template<typename T> inline math::Quaternion<T> operator*(const math::Quaternion<T>& a, const math::Quaternion<T>& b) {
		math::Quaternion<T> result = a;
		result *= b;
		return result;
	}

	// Stream operators for easy printing
	template<typename T, usize N> inline std::ostream& operator<<(std::ostream& os, const math::Vec<T, N>& vec) {
		os << "math::Vec" << N << "(";
		for (usize i = 0; i < N; ++i) {
			os << vec[i];
			if (i < N - 1)
				os << ", ";
		}
		os << ")";
		return os;
	}

	template<typename T, usize R, usize C> inline std::ostream& operator<<(std::ostream& os, const math::Mat<T, R, C>& mat) {
		os << "math::Mat" << R << "x" << C << "(\n";
		for (usize i = 0; i < R; ++i) {
			os << "  ";
			for (usize j = 0; j < C; ++j) {
				os << mat(i, j);
				if (j < C - 1)
					os << ", ";
			}
			os << "\n";
		}
		os << ")";
		return os;
	}

	template<typename T> inline std::ostream& operator<<(std::ostream& os, const math::Quaternion<T>& quat) {
		os << "Quat(" << quat.x << ", " << quat.y << ", " << quat.z << ", " << quat.w << ")";
		return os;
	}
} // namespace spark

#endif // SPARK_MATH_HPP