#ifndef SPARK_MATH_TYPES_HPP
#define SPARK_MATH_TYPES_HPP

#include "spark_pch.hpp"
#include <array>
#include <cassert>
#include <cmath>
#include <stdexcept>

namespace spark::math {

	// Forward declarations
	template<typename T, usize N> class Vec;
	template<typename T, usize Rows, usize Cols> class Mat;
	template<typename T> class Quaternion;

	// Helper storage for Vec specialization with named members.
	// For N == 2, 3, and 4 we define an anonymous union that provides both the
	// raw array and the named components (.x, .y, .z, .w). For other dimensions,
	// only the array is available.
	template<typename T, usize N>
	struct VecStorage {
		std::array<T, N> m_data;
	};

	template<typename T>
	struct VecStorage<T, 2> {
		union {
			std::array<T, 2> m_data;
			struct {
				T x;
				T y;
			};
		};
	};

	template<typename T>
	struct VecStorage<T, 3> {
		union {
			std::array<T, 3> m_data;
			struct {
				T x;
				T y;
				T z;
			};
		};
	};

	template<typename T>
	struct VecStorage<T, 4> {
		union {
			std::array<T, 4> m_data;
			struct {
				T x;
				T y;
				T z;
				T w;
			};
		};
	};

	// Inherits from VecStorage so that when N is 2, 3, or 4 you can access
	// the vector components using .x, .y, .z, .w in addition to using the array.
	template<typename T, usize N>
	class Vec : public VecStorage<T, N> {
	public:
		// Default constructor initializes all elements to T{}.
		Vec() {
			for (usize i = 0; i < N; i++)
				this->m_data[i] = T{};
		}

		// Fill constructor: set all components equal to s.
		explicit Vec(T s) {
			for (usize i = 0; i < N; i++)
				this->m_data[i] = s;
		}

		// Variadic constructor (requires exactly N arguments)
		template<typename... Args>
		Vec(Args... args)
			requires(sizeof...(Args) == N)
		{
			usize i = 0;
			((this->m_data[i++] = args), ...);
		}

		Vec(const Vec&) = default;
		Vec& operator=(const Vec&) = default;

		// Element access via operator[]
		T& operator[](usize index) { return this->m_data[index]; }
		const T& operator[](usize index) const { return this->m_data[index]; }

		// Basic arithmetic operators
		Vec operator+(const Vec& other) const {
			Vec result;
			for (usize i = 0; i < N; ++i)
				result[i] = this->m_data[i] + other.m_data[i];
			return result;
		}

		Vec operator-(const Vec& other) const {
			Vec result;
			for (usize i = 0; i < N; ++i)
				result[i] = this->m_data[i] - other.m_data[i];
			return result;
		}

		Vec operator*(T scalar) const {
			Vec result;
			for (usize i = 0; i < N; ++i)
				result[i] = this->m_data[i] * scalar;
			return result;
		}

		Vec operator/(T scalar) const {
			Vec result;
			for (usize i = 0; i < N; ++i)
				result[i] = this->m_data[i] / scalar;
			return result;
		}

		Vec operator*(const Vec& other) const {
			Vec result;
			for (usize i = 0; i < N; ++i)
				result[i] = this->m_data[i] * other.m_data[i];
			return result;
		}

		Vec operator/(const Vec& other) const {
			Vec result;
			for (usize i = 0; i < N; ++i) {
				if (other.m_data[i] == T{})
					throw std::runtime_error("Division by zero");
				result[i] = this->m_data[i] / other.m_data[i];
			}
			return result;
		}

		Vec& operator+=(const Vec& other) {
			for (usize i = 0; i < N; ++i)
				this->m_data[i] += other.m_data[i];
			return *this;
		}

		Vec& operator-=(const Vec& other) {
			for (usize i = 0; i < N; ++i)
				this->m_data[i] -= other.m_data[i];
			return *this;
		}

		Vec& operator*=(T scalar) {
			for (usize i = 0; i < N; ++i)
				this->m_data[i] *= scalar;
			return *this;
		}

		Vec& operator/=(T scalar) {
			for (usize i = 0; i < N; ++i)
				this->m_data[i] /= scalar;
			return *this;
		}

		Vec& operator*=(const Vec& other) {
			for (usize i = 0; i < N; ++i)
				this->m_data[i] *= other.m_data[i];
			return *this;
		}

		Vec& operator/=(const Vec& other) {
			for (usize i = 0; i < N; ++i) {
				if (other.m_data[i] == T{})
					throw std::runtime_error("Division by zero");
				this->m_data[i] /= other.m_data[i];
			}
			return *this;
		}

		Vec operator-() const {
			Vec result;
			for (usize i = 0; i < N; ++i)
				result[i] = -this->m_data[i];
			return result;
		}

		bool operator==(const Vec& other) const {
			for (usize i = 0; i < N; ++i)
				if (this->m_data[i] != other.m_data[i])
					return false;
			return true;
		}

		bool operator!=(const Vec& other) const { return !(*this == other); }

		// Vector operations
		T Dot(const Vec& other) const {
			T result = T{};
			for (usize i = 0; i < N; ++i)
				result += this->m_data[i] * other.m_data[i];
			return result;
		}

		// Cross product defined only for 3D vectors.
		Vec Cross(const Vec& other) const
			requires(N == 3)
		{
			return Vec(
				this->m_data[1] * other.m_data[2] - this->m_data[2] * other.m_data[1],
				this->m_data[2] * other.m_data[0] - this->m_data[0] * other.m_data[2],
				this->m_data[0] * other.m_data[1] - this->m_data[1] * other.m_data[0]
			);
		}

		T LengthSquared() const { return Dot(*this); }
		T Length() const { return std::sqrt(LengthSquared()); }
		Vec Normalized() const {
			T len = Length();
			return len == T{} ? *this : *this * (T{ 1 } / len);
		}

		// Returns the size (dimension) of the vector.
		constexpr usize Size() const { return N; }
		// Returns a pointer to the underlying data.
		const T* GetData() const { return this->m_data.data(); }
	};

	template<typename T, usize Rows, usize Cols>
	class Mat {
	public:
		Mat()
			: m_data{} {}

		explicit Mat(T scalar) {
			for (usize i = 0; i < Rows; ++i) {
				for (usize j = 0; j < Cols; ++j) {
					m_data[i][j] = (i == j) ? scalar : T{ 0 };
				}
			}
		}

		static Mat Identity()
			requires(Rows == Cols)
		{
			Mat result;
			for (usize i = 0; i < Rows; ++i)
				result.m_data[i][i] = T{ 1 };
			return result;
		}

		template<usize OtherCols>
		Mat<T, Rows, OtherCols> operator*(const Mat<T, Cols, OtherCols>& other) const {
			Mat<T, Rows, OtherCols> result;
			for (usize i = 0; i < Rows; ++i)
				for (usize j = 0; j < OtherCols; ++j) {
					T sum = T{};
					for (usize k = 0; k < Cols; ++k)
						sum += m_data[i][k] * other.m_data[k][j];
					result.m_data[i][j] = sum;
				}
			return result;
		}

		Vec<T, Rows> operator*(const Vec<T, Cols>& vec) const {
			Vec<T, Rows> result;
			for (usize i = 0; i < Rows; ++i) {
				T sum = T{};
				for (usize j = 0; j < Cols; ++j)
					sum += m_data[i][j] * vec[j];
				result[i] = sum;
			}
			return result;
		}

		Mat operator+(const Mat& other) const {
			Mat result;
			for (usize i = 0; i < Rows; ++i)
				for (usize j = 0; j < Cols; ++j)
					result.m_data[i][j] = m_data[i][j] + other.m_data[i][j];
			return result;
		}

		Mat operator-(const Mat& other) const {
			Mat result;
			for (usize i = 0; i < Rows; ++i)
				for (usize j = 0; j < Cols; ++j)
					result.m_data[i][j] = m_data[i][j] - other.m_data[i][j];
			return result;
		}

		Mat operator*(T scalar) const {
			Mat result;
			for (usize i = 0; i < Rows; ++i)
				for (usize j = 0; j < Cols; ++j)
					result.m_data[i][j] = m_data[i][j] * scalar;
			return result;
		}

		Mat& operator+=(const Mat& other) {
			for (usize i = 0; i < Rows; ++i)
				for (usize j = 0; j < Cols; ++j)
					m_data[i][j] += other.m_data[i][j];
			return *this;
		}

		Mat& operator-=(const Mat& other) {
			for (usize i = 0; i < Rows; ++i)
				for (usize j = 0; j < Cols; ++j)
					m_data[i][j] -= other.m_data[i][j];
			return *this;
		}

		Mat& operator*=(T scalar) {
			for (usize i = 0; i < Rows; ++i)
				for (usize j = 0; j < Cols; ++j)
					m_data[i][j] *= scalar;
			return *this;
		}

		template<usize OtherCols>
		Mat& operator*=(const Mat<T, Cols, OtherCols>& other) {
			*this = *this * other;
			return *this;
		}

		Mat operator-() const {
			Mat result;
			for (usize i = 0; i < Rows; ++i)
				for (usize j = 0; j < Cols; ++j)
					result.m_data[i][j] = -m_data[i][j];
			return result;
		}

		std::array<T, Cols>& operator[](usize row) {
			assert(row < Rows);
			return m_data[row];
		}

		const std::array<T, Cols>& operator[](usize row) const {
			assert(row < Rows);
			return m_data[row];
		}

		bool operator==(const Mat& other) const {
			for (usize i = 0; i < Rows; ++i)
				for (usize j = 0; j < Cols; ++j)
					if (m_data[i][j] != other.m_data[i][j])
						return false;
			return true;
		}

		bool operator!=(const Mat& other) const { return !(*this == other); }

		Mat<T, Cols, Rows> Transpose() const {
			Mat<T, Cols, Rows> result;
			for (usize i = 0; i < Rows; ++i)
				for (usize j = 0; j < Cols; ++j)
					result.m_data[j][i] = m_data[i][j];
			return result;
		}

		T& operator()(usize row, usize col) { return m_data[row][col]; }
		const T& operator()(usize row, usize col) const { return m_data[row][col]; }

		const T* Data() const
		{
			return &m_data[0][0];
		}

		T* Data()
		{
			return &m_data[0][0];
		}

	private:
		std::array<std::array<T, Cols>, Rows> m_data;
	};

	template<typename T>
	class Quaternion {
	public:
		Quaternion()
			: x(T{}), y(T{}), z(T{}), w(T{ 1 }) {}
		Quaternion(T x, T y, T z, T w)
			: x(x), y(y), z(z), w(w) {}
		explicit Quaternion(const Vec<T, 3>& euler);
		explicit Quaternion(const Vec<T, 3>& axis, T angle);

		Quaternion operator*(const Quaternion& other) const {
			return Quaternion(
				w * other.x + x * other.w + y * other.z - z * other.y,
				w * other.y - x * other.z + y * other.w + z * other.x,
				w * other.z + x * other.y - y * other.x + z * other.w,
				w * other.w - x * other.x - y * other.y - z * other.z
			);
		}

		Quaternion operator*(T scalar) const { return Quaternion(x * scalar, y * scalar, z * scalar, w * scalar); }
		Quaternion operator+(const Quaternion& other) const { return Quaternion(x + other.x, y + other.y, z + other.z, w + other.w); }
		Quaternion operator-(const Quaternion& other) const { return Quaternion(x - other.x, y - other.y, z - other.z, w - other.w); }

		Quaternion& operator+=(const Quaternion& other) {
			x += other.x;
			y += other.y;
			z += other.z;
			w += other.w;
			return *this;
		}

		Quaternion& operator-=(const Quaternion& other) {
			x -= other.x;
			y -= other.y;
			z -= other.z;
			w -= other.w;
			return *this;
		}

		Quaternion& operator*=(T scalar) {
			x *= scalar;
			y *= scalar;
			z *= scalar;
			w *= scalar;
			return *this;
		}

		Quaternion& operator*=(const Quaternion& other) {
			T new_x = w * other.x + x * other.w + y * other.z - z * other.y;
			T new_y = w * other.y - x * other.z + y * other.w + z * other.x;
			T new_z = w * other.z + x * other.y - y * other.x + z * other.w;
			T new_w = w * other.w - x * other.x - y * other.y - z * other.z;
			x = new_x; y = new_y; z = new_z; w = new_w;
			return *this;
		}

		Quaternion operator-() const { return Quaternion(-x, -y, -z, -w); }

		bool operator==(const Quaternion& other) const { return x == other.x && y == other.y && z == other.z && w == other.w; }
		bool operator!=(const Quaternion& other) const { return !(*this == other); }

		Vec<T, 3> RotateVector(const Vec<T, 3>& v) const {
			Vec<T, 3> q_vec(x, y, z);
			Vec<T, 3> t = q_vec.Cross(v) * T { 2 };
			return v + t * w + q_vec.Cross(t);
		}

		T Length() const { return std::sqrt(x * x + y * y + z * z + w * w); }
		Quaternion Normalized() const {
			T len = Length();
			return len == T{} ? *this : *this * (T{ 1 } / len);
		}
		Quaternion Conjugate() const { return Quaternion(-x, -y, -z, w); }
		Mat<T, 3, 3> ToMat3() const;
		Mat<T, 4, 4> ToMat4() const;
		Vec<T, 3> ToEuler() const;

		T x;
		T y;
		T z;
		T w;
	};

	// Common type aliases
	using Vec2f = Vec<f32, 2>;
	using Vec2d = Vec<f64, 2>;
	using Vec2i = Vec<i32, 2>;
	using Vec2u = Vec<u32, 2>;

	using Vec3f = Vec<f32, 3>;
	using Vec3d = Vec<f64, 3>;
	using Vec3i = Vec<i32, 3>;
	using Vec3u = Vec<u32, 3>;

	using Vec4f = Vec<f32, 4>;
	using Vec4d = Vec<f64, 4>;
	using Vec4i = Vec<i32, 4>;
	using Vec4u = Vec<u32, 4>;

	using Mat2f = Mat<f32, 2, 2>;
	using Mat3f = Mat<f32, 3, 3>;
	using Mat4f = Mat<f32, 4, 4>;

	using Mat2d = Mat<f64, 2, 2>;
	using Mat3d = Mat<f64, 3, 3>;
	using Mat4d = Mat<f64, 4, 4>;

	using Quatf = Quaternion<f32>;
	using Quatd = Quaternion<f64>;

	using Vec2 = Vec2f;
	using Vec3 = Vec3f;
	using Vec4 = Vec4f;

	using Mat2 = Mat2f;
	using Mat3 = Mat3f;
	using Mat4 = Mat4f;

	using Quat = Quatf;

} // namespace spark::math

#endif // SPARK_MATH_TYPES_HPP
