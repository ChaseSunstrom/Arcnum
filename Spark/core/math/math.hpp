#ifndef SPARK_MATH_HPP
#define SPARK_MATH_HPP

#include <core/pch.hpp>
#include <include/glm/geometric.hpp>
#include <include/glm/gtc/random.hpp>
#include <include/glm/gtx/norm.hpp>
#include <include/glm/common.hpp>
#include <include/glm/glm.hpp>
#include <include/glm/gtc/matrix_transform.hpp>
#include <include/glm/gtc/noise.hpp>
#include <include/glm/vector_relational.hpp>
#include <include/glm/gtc/quaternion.hpp>
#include <include/glm/gtc/type_ptr.hpp>
#include <include/glm/gtx/matrix_interpolation.hpp>
#include <include/glm/gtx/euler_angles.hpp>
#include <include/glm/gtx/vector_angle.hpp>
#include <include/glm/gtx/color_space.hpp>
#include <include/glm/gtx/compatibility.hpp>
#include <include/glm/gtx/euler_angles.hpp>
#include <include/glm/gtx/matrix_decompose.hpp>
#include <include/glm/gtx/norm.hpp>
#include <include/glm/gtx/projection.hpp>
#include <include/glm/gtx/quaternion.hpp>
#include <include/glm/gtx/rotate_vector.hpp>
#include <include/glm/gtx/spline.hpp>
#include <include/glm/gtx/vector_angle.hpp>

namespace Spark::Math {
	using Vec2            = glm::vec2;
	using Vec3            = glm::vec3;
	using Vec4            = glm::vec4;
	using Mat2            = glm::mat2;
	using Mat3            = glm::mat3;
	using Mat4            = glm::mat4;
	using Quat            = glm::quat;

	// Constants
	constexpr f32 PI      = glm::pi<f32>();
	constexpr f32 EPSILON = glm::epsilon<f32>();
	constexpr f32 TWO_PI  = 2.0f * PI;
	constexpr f32 HALF_PI = 0.5f * PI;
	constexpr f32 E       = 2.71828182845904523536f;
	constexpr f32 LOG2E   = 1.44269504088896340736f;
	constexpr f32 LOG10E  = 0.434294481903251827651f;
	constexpr f32 LN2     = 0.693147180559945309417f;
	constexpr f32 LN10    = 2.30258509299404568402f;
	constexpr f32 SQRT2   = 1.41421356237309504880f;
	constexpr f32 SQRT1_2 = 0.707106781186547524401f;

	// Basic arithmetic
	template<typename T> inline T Add(T a, T b) { return a + b; }

	template<typename T> inline T Subtract(T a, T b) { return a - b; }

	template<typename T> inline T Multiply(T a, T b) { return a * b; }

	template<typename T> inline T Divide(T a, T b) { return a / b; }

	// Power and root functions
	inline f32 Pow(f32 base, f32 exponent) { return std::pow(base, exponent); }
	inline f32 Sqrt(f32 x) { return std::sqrt(x); }
	inline f32 CubeRoot(f32 x) { return std::cbrt(x); }

	// Exponential and logarithmic functions
	inline f32 Exp(f32 x) { return std::exp(x); }
	inline f32 Log(f32 x) { return std::log(x); }
	inline f32 Log2(f32 x) { return std::log2(x); }
	inline f32 Log10(f32 x) { return std::log10(x); }

	// Trigonometric functions (in radians)
	inline f32 Sin(f32 x) { return std::sin(x); }
	inline f32 Cos(f32 x) { return std::cos(x); }
	inline f32 Tan(f32 x) { return std::tan(x); }
	inline f32 Asin(f32 x) { return std::asin(x); }
	inline f32 Acos(f32 x) { return std::acos(x); }
	inline f32 Atan(f32 x) { return std::atan(x); }
	inline f32 Atan2(f32 y, f32 x) { return std::atan2(y, x); }

	// Hyperbolic functions
	inline f32 Sinh(f32 x) { return std::sinh(x); }
	inline f32 Cosh(f32 x) { return std::cosh(x); }
	inline f32 Tanh(f32 x) { return std::tanh(x); }
	inline f32 Asinh(f32 x) { return std::asinh(x); }
	inline f32 Acosh(f32 x) { return std::acosh(x); }
	inline f32 Atanh(f32 x) { return std::atanh(x); }

	// Rounding and remainder functions
	inline f32 Ceil(f32 x) { return std::ceil(x); }
	inline f32 Floor(f32 x) { return std::floor(x); }
	inline f32 Round(f32 x) { return std::round(x); }
	inline f32 Trunc(f32 x) { return std::trunc(x); }
	inline f32 Fmod(f32 x, f32 y) { return std::fmod(x, y); }

	// Absolute value and sign
	template<typename T> inline T Abs(T x) { return std::abs(x); }
	template<typename T> inline T Sign(T x) { return (T(0) < x) - (x < T(0)); }

	// Min and Max
	template<typename T> inline T Min(T a, T b) { return (a < b) ? a : b; }
	template<typename T> inline T Max(T a, T b) { return (a > b) ? a : b; }

	// Clamping
	template<typename T> inline T Clamp(T x, T min, T max) { return Min(Max(x, min), max); }

	// Interpolation
	template<typename T> inline T Lerp(T a, T b, f32 t) { return a + t * (b - a); }

	// Factorial (for integers)
	inline i32 Factorial(i32 n) {
		if (n <= 1)
			return 1;
		return n * Factorial(n - 1);
	}

	// Permutations and Combinations
	inline i32 Permutations(i32 n, i32 r) { return Factorial(n) / Factorial(n - r); }

	inline i32 Combinations(i32 n, i32 r) { return Permutations(n, r) / Factorial(r); }

	// Greatest Common Divisor (GCD) using Euclidean algorithm
	inline i32 GCD(i32 a, i32 b) {
		while (b != 0) {
			i32 temp = b;
			b        = a % b;
			a        = temp;
		}
		return a;
	}

	// Least Common Multiple (LCM)
	inline i32 LCM(i32 a, i32 b) { return (a * b) / GCD(a, b); }

	// Prime number check
	inline bool IsPrime(i32 n) {
		if (n <= 1)
			return false;
		for (i32 i = 2; i * i <= n; i++) {
			if (n % i == 0)
				return false;
		}
		return true;
	}

	// Matrix functions
	inline Mat4 Perspective(f32 fov, f32 aspect, f32 near, f32 far) { return glm::perspective(fov, aspect, near, far); }
	inline Mat4 Orthographic(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far) { return glm::ortho(left, right, bottom, top, near, far); }
	inline Mat4 LookAt(const Vec3& eye, const Vec3& center, const Vec3& up) { return glm::lookAt(eye, center, up); }
	inline Mat4 Translate(const Mat4& matrix, const Vec3& translation) { return glm::translate(matrix, translation); }
	inline Mat4 Rotate(const Mat4& matrix, const Vec3& rotation) {
		Mat4 result = matrix;
		result      = glm::rotate(result, rotation.x, Vec3(1, 0, 0));
		result      = glm::rotate(result, rotation.y, Vec3(0, 1, 0));
		result      = glm::rotate(result, rotation.z, Vec3(0, 0, 1));
		return result;
	}
	inline Mat4 Rotate(const Mat4& matrix, f32 angle, const Vec3& axis) { return glm::rotate(matrix, angle, axis); }
	inline Mat4 Scale(const Mat4& matrix, const Vec3& scale) { return glm::scale(matrix, scale); }
	inline Mat4 Inverse(const Mat4& matrix) { return glm::inverse(matrix); }
	inline Mat4 Transpose(const Mat4& matrix) { return glm::transpose(matrix); }
	inline Mat4 Identity() { return glm::mat4(1.0f); }
	inline Mat4 ScaleFromVector(const Vec3& scale) { return glm::scale(Mat4(1.0f), scale); }
	inline Mat4 TranslationFromVector(const Vec3& translation) { return glm::translate(Mat4(1.0f), translation); }

	// Vector functions
	inline f32  Dot(const Vec2& a, const Vec2& b) { return glm::dot(a, b); }
	inline f32  Dot(const Vec3& a, const Vec3& b) { return glm::dot(a, b); }
	inline f32  Dot(const Vec4& a, const Vec4& b) { return glm::dot(a, b); }
	inline Vec3 Cross(const Vec3& a, const Vec3& b) { return glm::cross(a, b); }
	inline f32  Length(const Vec2& v) { return glm::length(v); }
	inline f32  Length(const Vec3& v) { return glm::length(v); }
	inline f32  Length(const Vec4& v) { return glm::length(v); }
	inline Vec2 Normalize(const Vec2& v) { return glm::normalize(v); }
	inline Vec3 Normalize(const Vec3& v) { return glm::normalize(v); }
	inline Vec4 Normalize(const Vec4& v) { return glm::normalize(v); }
	inline Vec3 Reflect(const Vec3& incident, const Vec3& normal) { return glm::reflect(incident, normal); }
	inline Vec3 Refract(const Vec3& incident, const Vec3& normal, f32 eta) { return glm::refract(incident, normal, eta); }
	inline Vec3 Transform(const Vec3& v, const Mat4& matrix) { return glm::vec3(matrix * glm::vec4(v, 1.0f)); }
	inline Vec4 Transform(const Vec4& v, const Mat4& matrix) { return matrix * v; }
	inline Vec3 TransformDirection(const Vec3& v, const Mat4& matrix) { return glm::normalize(glm::vec3(matrix * glm::vec4(v, 0.0f))); }
	inline Vec3 TransformNormal(const Vec3& v, const Mat4& matrix) { return glm::normalize(glm::vec3(glm::transpose(glm::inverse(matrix)) * glm::vec4(v, 0.0f))); }
	inline Vec3 Lerp(const Vec3& a, const Vec3& b, f32 t) { return glm::lerp(a, b, t); }
	inline Vec3 Nlerp(const Vec3& a, const Vec3& b, f32 t) { return glm::normalize(glm::lerp(a, b, t)); }
	inline Vec3 Clamp(const Vec3& v, f32 min, f32 max) { return glm::clamp(v, min, max); }
	inline Vec3 Clamp(const Vec3& v, const Vec3& min, const Vec3& max) { return glm::clamp(v, min, max); }
	inline Vec3 Pow(const Vec3& v, f32 exponent) { return glm::pow(v, Vec3(exponent)); }
	inline Vec3 Max(const Vec3& a, const Vec3& b) { return glm::max(a, b); }
	inline Vec3 Min(const Vec3& a, const Vec3& b) { return glm::min(a, b); }
	inline Vec3 Abs(const Vec3& v) { return glm::abs(v); }
	inline Vec3 Round(const Vec3& v) { return glm::round(v); }
	inline Vec3 Ceil(const Vec3& v) { return glm::ceil(v); }
	inline Vec3 Floor(const Vec3& v) { return glm::floor(v); }
	inline Vec3 Fract(const Vec3& v) { return glm::fract(v); }
	inline Vec3 Mix(const Vec3& a, const Vec3& b, f32 t) { return glm::mix(a, b, t); }
	inline Vec3 Step(const Vec3& edge, const Vec3& x) { return glm::step(edge, x); }
	inline Vec3 SmoothStep(const Vec3& edge0, const Vec3& edge1, const Vec3& x) { return glm::smoothstep(edge0, edge1, x); }
	inline Vec3 Sign(const Vec3& v) { return glm::sign(v); }
	inline Vec3 Mod(const Vec3& x, f32 y) { return glm::mod(x, y); }
	inline Vec3 Mod(const Vec3& x, const Vec3& y) { return glm::mod(x, y); }
	inline Vec3 Exp(const Vec3& v) { return glm::exp(v); }
	inline Vec3 Log(const Vec3& v) { return glm::log(v); }
	inline Vec3 Sqrt(const Vec3& v) { return glm::sqrt(v); }
	inline Vec3 InverseSqrt(const Vec3& v) { return glm::inversesqrt(v); }
	inline Vec3 Sin(const Vec3& v) { return glm::sin(v); }
	inline Vec3 Cos(const Vec3& v) { return glm::cos(v); }
	inline Vec3 Tan(const Vec3& v) { return glm::tan(v); }
	inline Vec3 Asin(const Vec3& v) { return glm::asin(v); }
	inline Vec3 Acos(const Vec3& v) { return glm::acos(v); }
	inline Vec3 Atan(const Vec3& v) { return glm::atan(v); }
	inline Vec3 Atan2(const Vec3& y, const Vec3& x) { return glm::atan(y, x); }
	inline Vec3 Degrees(const Vec3& v) { return glm::degrees(v); }
	inline Vec3 Radians(const Vec3& v) { return glm::radians(v); }
	inline Vec3 Project(const Vec3& v, const Vec3& normal) { return glm::proj(v, glm::normalize(normal)); }
	inline Vec3 Reject(const Vec3& v, const Vec3& normal) { return v - Project(v, normal); }
	inline Vec3 QuaternionToEuler(const Quat& q) { return glm::degrees(glm::eulerAngles(q)); }
	inline Vec3 EulerAngles(const Quat& q) { return glm::eulerAngles(q); }
	inline Vec3 RotateVector(const Vec3& v, const Quat& q) { return glm::rotate(q, v); }
	inline f32  Distance(const Vec3& a, const Vec3& b) { return glm::distance(a, b); }
	inline f32  DistanceSquared(const Vec3& a, const Vec3& b) { return glm::distance2(a, b); }
	inline Vec3 ProjectOnPlane(const Vec3& v, const Vec3& plane_normal) { return v - Project(v, plane_normal); }
	inline f32  AngleBetween(const Vec3& a, const Vec3& b) { return Math::Acos(Dot(Normalize(a), Normalize(b))); }

	// Quat functions
	inline Quat EulerToQuaternion(const Vec3& euler) { return glm::quat(glm::vec3(glm::radians(euler.x), glm::radians(euler.y), glm::radians(euler.z))); }
	inline Quat MatrixToQuaternion(const Mat4& matrix) { return glm::quat_cast(matrix); }
	inline Quat Slerp(const Quat& a, const Quat& b, f32 t) { return glm::slerp(a, b, t); }
	inline Quat LookAtQuaternion(const Vec3& direction, const Vec3& up = Vec3(0, 1, 0)) { return glm::quatLookAt(direction, up); }
	inline Quat RotationBetweenVectors(const Vec3& start, const Vec3& dest) { return glm::rotation(Normalize(start), Normalize(dest)); }

	// Angle functions
	inline f32 Angle(const Vec3& a, const Vec3& b) { return glm::angle(a, b); }
	inline f32 OrientedAngle(const Vec3& a, const Vec3& b, const Vec3& ref) { return glm::orientedAngle(a, b, ref); }
	inline f32 Radians(f32 degrees) { return glm::radians(degrees); }
	inline f32 Degrees(f32 radians) { return glm::degrees(radians); }

	// Random number generation
	inline f32  Random() { return glm::linearRand(0.0f, 1.0f); }
	inline f32  RandomRange(f32 min, f32 max) { return glm::linearRand(min, max); }
	inline Vec3 RandomVec3() { return glm::sphericalRand(1.0f); }
	inline Vec3 RandomVec3InCube() { return glm::linearRand(Vec3(-1), Vec3(1)); }

	// Intersection tests
	inline bool RayPlaneIntersection(const Vec3& ray_origin, const Vec3& ray_dir, const Vec3& plane_normal, const Vec3& plane_point, f32& t) {
		f32 denom = Dot(plane_normal, ray_dir);
		if (Math::Abs(denom) > EPSILON) {
			Vec3 p0l0 = plane_point - ray_origin;
			t         = Dot(p0l0, plane_normal) / denom;
			return (t >= 0);
		}
		return false;
	}

	inline bool RaySphereIntersection(const Vec3& ray_origin, const Vec3& ray_dir, const Vec3& sphere_center, f32 sphere_radius, f32& t) {
		Vec3 oc           = ray_origin - sphere_center;
		f32  a            = Dot(ray_dir, ray_dir);
		f32  b            = 2.0f * Dot(oc, ray_dir);
		f32  c            = Dot(oc, oc) - sphere_radius * sphere_radius;
		f32  discriminant = b * b - 4 * a * c;

		if (discriminant < 0)
			return false;

		t = (-b - Math::Sqrt(discriminant)) / (2.0f * a);
		return t >= 0;
	}

	// Color conversion functions
	inline Vec3 HSVtoRGB(const Vec3& hsv) { return glm::rgbColor(hsv); }
	inline Vec3 RGBtoHSV(const Vec3& rgb) { return glm::hsvColor(rgb); }

	// Easing functions
	inline f32 EaseInQuad(f32 t) { return t * t; }
	inline f32 EaseOutQuad(f32 t) { return t * (2 - t); }
	inline f32 EaseInOutQuad(f32 t) { return t < 0.5f ? 2 * t * t : -1 + (4 - 2 * t) * t; }
	inline f32 EaseInCubic(f32 t) { return t * t * t; }
	inline f32 EaseOutCubic(f32 t) {
		f32 t1 = t - 1;
		return t1 * t1 * t1 + 1;
	}
	inline f32 EaseInSine(f32 t) { return 1.0f - Math::Cos(t * HALF_PI); }
	inline f32 EaseOutSine(f32 t) { return Math::Sin(t * HALF_PI); }
	inline f32 EaseInOutSine(f32 t) { return -0.5f * (Math::Cos(PI * t) - 1.0f); }
	inline f32 EaseInOutCubic(f32 t) { return t < 0.5f ? 4 * t * t * t : (t - 1) * (2 * t - 2) * (2 * t - 2) + 1; }

	// Noise functions
	inline f32 PerlinNoise(const Vec3& v) { return glm::perlin(v); }
	inline f32 SimplexNoise(const Vec3& v) { return glm::simplex(v); }
	inline f32 FractalBrownianMotion(const Vec3& v, i32 octaves, f32 lacunarity = 2.0f, f32 gain = 0.5f) {
		f32  value     = 0.0f;
		f32  amplitude = 1.0f;
		Vec3 frequency = v;

		for (i32 i = 0; i < octaves; ++i) {
			value += amplitude * PerlinNoise(frequency);
			frequency *= lacunarity;
			amplitude *= gain;
		}

		return value;
	}

	// Additional utility functions
	inline f32  Clamp01(f32 value) { return glm::clamp(value, 0.0f, 1.0f); }
	inline Vec3 Clamp01(const Vec3& v) { return glm::clamp(v, Vec3(0), Vec3(1)); }
	inline f32  Saturate(f32 value) { return Clamp01(value); }
	inline Vec3 Saturate(const Vec3& v) { return Clamp01(v); }

	// Interpolation functions
	inline Vec3 CatmullRom(const Vec3& v1, const Vec3& v2, const Vec3& v3, const Vec3& v4, f32 t) { return glm::catmullRom(v1, v2, v3, v4, t); }
	inline Vec3 Hermite(const Vec3& v1, const Vec3& t1, const Vec3& v2, const Vec3& t2, f32 t) { return glm::hermite(v1, t1, v2, t2, t); }
	inline Vec3 BezierCurve(const Vec3& p0, const Vec3& p1, const Vec3& p2, f32 t) {
		f32 u = 1.0f - t;
		return u * u * p0 + 2.0f * u * t * p1 + t * t * p2;
	}

	inline Vec3 CubicBezierCurve(const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec3& p3, f32 t) {
		f32 u = 1.0f - t;
		return u * u * u * p0 + 3.0f * u * u * t * p1 + 3.0f * u * t * t * p2 + t * t * t * p3;
	}

	// Geometric functions
	inline Vec3 TriangleNormal(const Vec3& a, const Vec3& b, const Vec3& c) { return Normalize(Cross(b - a, c - a)); }
	inline f32  TriangleArea(const Vec3& a, const Vec3& b, const Vec3& c) { return Length(Cross(b - a, c - a)) * 0.5f; }
	inline bool PointInTriangle(const Vec3& p, const Vec3& a, const Vec3& b, const Vec3& c) {
		Vec3 v0      = c - a;
		Vec3 v1      = b - a;
		Vec3 v2      = p - a;

		f32 dot00    = Dot(v0, v0);
		f32 dot01    = Dot(v0, v1);
		f32 dot02    = Dot(v0, v2);
		f32 dot11    = Dot(v1, v1);
		f32 dot12    = Dot(v1, v2);

		f32 invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
		f32 u        = (dot11 * dot02 - dot01 * dot12) * invDenom;
		f32 v        = (dot00 * dot12 - dot01 * dot02) * invDenom;

		return (u >= 0) && (v >= 0) && (u + v < 1);
	}

	// Conversion functions
	inline Vec3 CartesianToSpherical(const Vec3& cartesian) {
		f32 r     = Length(cartesian);
		f32 theta = Math::Acos(cartesian.z / r);
		f32 phi   = Math::Atan2(cartesian.y, cartesian.x);
		return Vec3(r, theta, phi);
	}
	inline Vec3 SphericalToCartesian(const Vec3& spherical) {
		f32 r = spherical.x, theta = spherical.y, phi = spherical.z;
		f32 sin_theta = Math::Sin(theta);
		return Vec3(r * sin_theta * Math::Cos(phi), r * sin_theta * Math::Sin(phi), r * Math::Cos(theta));
	}

	// Matrix decomposition
	inline void DecomposeTransform(const Mat4& transform, Vec3& translation, Quat& rotation, Vec3& scale) {
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(transform, scale, rotation, translation, skew, perspective);
	}

	// Frustum functions
	inline bool PointInFrustum(const Vec3& point, const Mat4& view_projection) {
		Vec4 clip_space = view_projection * Vec4(point, 1.0f);
		return Math::Abs(clip_space.x) <= clip_space.w && Math::Abs(clip_space.y) <= clip_space.w && 0 <= clip_space.z && clip_space.z <= clip_space.w;
	}

	// Vector casts
	inline Vec2 Vec2Cast(const Vec3& v) { return Vec2(v); }
	inline Vec2 Vec2Cast(const Vec4& v) { return Vec2(v); }
	inline Vec3 Vec3Cast(const Vec2& v, f32 z = 0.0f) { return Vec3(v, z); }
	inline Vec3 Vec3Cast(const Vec4& v) { return Vec3(v); }
	inline Vec4 Vec4Cast(const Vec2& v, f32 z = 0.0f, f32 w = 1.0f) { return Vec4(v, z, w); }
	inline Vec4 Vec4Cast(const Vec3& v, f32 w = 1.0f) { return Vec4(v, w); }

	// Matrix casts
	inline Mat3 Mat3Cast(const Mat4& m) { return Mat3(m); }
	inline Mat4 Mat4Cast(const Mat3& m) { return Mat4(m); }
	inline Mat4 Mat4Cast(const Quat& q) { return glm::mat4_cast(q); }
	inline Mat3 Mat3Cast(const Quat& q) { return glm::mat3_cast(q); }

	// Quaternion casts
	inline Quat QuatCast(const Mat3& m) { return glm::quat_cast(m); }
	inline Quat QuatCast(const Mat4& m) { return glm::quat_cast(m); }
	inline Quat QuatCast(const Vec3& eulerAngles) { return glm::quat(eulerAngles); }
	inline Quat QuatCast(const Vec4& axisAngle) { return glm::angleAxis(axisAngle.w, Vec3(axisAngle)); }

	// Euler angle conversions
	inline Vec3 EulerCast(const Quat& q) { return glm::eulerAngles(q); }
	inline Vec3 EulerCast(const Mat3& m) { return glm::eulerAngles(QuatCast(m)); }
	inline Vec3 EulerCast(const Mat4& m) { return glm::eulerAngles(QuatCast(m)); }

	// Axis-angle conversions
	inline Vec4 AxisAngleCast(const Quat& q) {
		Vec3 axis;
		f32  angle;
		glm::axisAngle(Math::Mat4Cast(q), axis, angle);
		return Vec4(axis, angle);
	}

	inline Vec4 AxisAngleCast(const Mat3& m) { return AxisAngleCast(QuatCast(m)); }
	inline Vec4 AxisAngleCast(const Mat4& m) { return AxisAngleCast(QuatCast(m)); }

} // namespace Spark::Math

#endif