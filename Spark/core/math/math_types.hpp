#ifndef SPARK_MATH_TYPES_HPP
#define SPARK_MATH_TYPES_HPP

#include <cmath>
#include <core/util/container/array.hpp>
#include <core/util/log.hpp>
#include <core/util/types.hpp>

namespace Spark {
	// Forward declarations
	template<typename T, usize N> class Vec;
	template<typename T, usize Rows, usize Cols> class Mat;
	template<typename T> class Quaternion;

	// Vector class implementation
	template<typename T, usize N> class Vec {
	  public:
		Vec()
			: m_data{} {}

		explicit Vec(T s) {
			for (usize i = 0; i < N; i++)
				m_data[i] = s;
		}

		template<typename... Args> Vec(Args... args)
			requires(sizeof...(Args) == N)
		{
			usize i = 0;
			((m_data[i++] = args), ...);
		}

		Vec(const Vec&)            = default;
		Vec& operator=(const Vec&) = default;

		T&       operator[](usize index) { return m_data[index]; }
		const T& operator[](usize index) const { return m_data[index]; }

		// Basic arithmetic operators
		Vec operator+(const Vec& other) const {
			Vec result;
			for (usize i = 0; i < N; ++i)
				result.m_data[i] = m_data[i] + other.m_data[i];
			return result;
		}
		Vec operator-(const Vec& other) const {
			Vec result;
			for (usize i = 0; i < N; ++i)
				result.m_data[i] = m_data[i] - other.m_data[i];
			return result;
		}
		Vec operator*(T scalar) const {
			Vec result;
			for (usize i = 0; i < N; ++i)
				result.m_data[i] = m_data[i] * scalar;
			return result;
		}
		Vec operator/(T scalar) const {
			Vec result;
			for (usize i = 0; i < N; ++i)
				result.m_data[i] = m_data[i] / scalar;
			return result;
		}
		Vec operator*(const Vec& other) const {
			Vec result;
			for (usize i = 0; i < N; ++i)
				result.m_data[i] = m_data[i] * other.m_data[i];
			return result;
		}
		Vec operator/(const Vec& other) const {
			Vec result;
			for (usize i = 0; i < N; ++i) {
				if (other.m_data[i] == T{})
					LOG_FATAL("Division by zero");
				result.m_data[i] = m_data[i] / other.m_data[i];
			}
			return result;
		}

		// Compound assignment operators
		Vec& operator+=(const Vec& other) {
			for (usize i = 0; i < N; ++i)
				m_data[i] += other.m_data[i];
			return *this;
		}

		Vec& operator-=(const Vec& other) {
			for (usize i = 0; i < N; ++i)
				m_data[i] -= other.m_data[i];
			return *this;
		}

		Vec& operator*=(T scalar) {
			for (usize i = 0; i < N; ++i)
				m_data[i] *= scalar;
			return *this;
		}

		Vec& operator/=(T scalar) {
			for (usize i = 0; i < N; ++i)
				m_data[i] /= scalar;
			return *this;
		}

		Vec& operator*=(const Vec& other) {
			for (usize i = 0; i < N; ++i)
				m_data[i] *= other.m_data[i];
			return *this;
		}

		Vec& operator/=(const Vec& other) {
			for (usize i = 0; i < N; ++i) {
				if (other.m_data[i] == T{})
					throw std::runtime_error("Division by zero");
				m_data[i] /= other.m_data[i];
			}
			return *this;
		}

		Vec operator-() const {
			Vec result;
			for (usize i = 0; i < N; ++i)
				result.m_data[i] = -m_data[i];
			return result;
		}

		bool operator==(const Vec& other) const {
			for (usize i = 0; i < N; ++i)
				if (m_data[i] != other.m_data[i])
					return false;
			return true;
		}

		bool operator!=(const Vec& other) const { return !(*this == other); }

		// Vector operations
		T Dot(const Vec& other) const {
			T result = T{};
			for (usize i = 0; i < N; ++i)
				result += m_data[i] * other.m_data[i];
			return result;
		}

		Vec Cross(const Vec& other) const
			requires(N == 3)
		{
			return Vec(m_data[1] * other.m_data[2] - m_data[2] * other.m_data[1], m_data[2] * other.m_data[0] - m_data[0] * other.m_data[2], m_data[0] * other.m_data[1] - m_data[1] * other.m_data[0]);
		}

		T   LengthSquared() const { return Dot(*this); }
		T   Length() const { return std::sqrt(LengthSquared()); }
		Vec Normalized() const {
			T len = Length();
			return len == T{} ? *this : *this * (T{1} / len);
		}

		// Component access
		T& X()
			requires(N >= 1)
		{
			return m_data[0];
		}
		T& Y()
			requires(N >= 2)
		{
			return m_data[1];
		}
		T& Z()
			requires(N >= 3)
		{
			return m_data[2];
		}
		T& W()
			requires(N >= 4)
		{
			return m_data[3];
		}

		T& R()
			requires(N >= 1)
		{
			return m_data[0];
		}
		T& G()
			requires(N >= 2)
		{
			return m_data[1];
		}
		T& B()
			requires(N >= 3)
		{
			return m_data[2];
		}
		T& A()
			requires(N >= 4)
		{
			return m_data[3];
		}

		const T& X() const
			requires(N >= 1)
		{
			return m_data[0];
		}
		const T& Y() const
			requires(N >= 2)
		{
			return m_data[1];
		}
		const T& Z() const
			requires(N >= 3)
		{
			return m_data[2];
		}
		const T& W() const
			requires(N >= 4)
		{
			return m_data[3];
		}

		const T& R() const
			requires(N >= 1)
		{
			return m_data[0];
		}
		const T& G() const
			requires(N >= 2)
		{
			return m_data[1];
		}
		const T& B() const
			requires(N >= 3)
		{
			return m_data[2];
		}
		const T& A() const
			requires(N >= 4)
		{
			return m_data[3];
		}

		constexpr usize Size() const { return N; }
		const T*        GetData() const { return m_data.data(); }

	  private:
		Array<T, N> m_data;
		template<typename U, usize R, usize C> friend class Mat;
		template<typename U> friend class Quaternion;
	};

	// Matrix class implementation
	template<typename T, usize Rows, usize Cols> class Mat {
	  public:
		Mat()
			: m_data{} {}

		explicit Mat(T scalar) {
			for (usize i = 0; i < Rows; ++i) {
				for (usize j = 0; j < Cols; ++j) {
					m_data[i][j] = (i == j) ? scalar : T{0};
				}
			}
		}

		static Mat Identity()
			requires(Rows == Cols)
		{
			Mat result;
			for (usize i = 0; i < Rows; ++i)
				result.m_data[i][i] = T{1};
			return result;
		}

		// Matrix operations
		template<usize OtherCols> Mat<T, Rows, OtherCols> operator*(const Mat<T, Cols, OtherCols>& other) const {
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

		template<usize OtherCols> Mat& operator*=(const Mat<T, Cols, OtherCols>& other) {
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

		Array<T, Cols>& operator[](usize row) {
			assert(row < Rows);
			return m_data[row];
		}

		// Const version for read-only access
		const Array<T, Cols>& operator[](usize row) const {
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

		T&       operator()(usize row, usize col) { return m_data[row][col]; }
		const T& operator()(usize row, usize col) const { return m_data[row][col]; }

	  private:
		Array<Array<T, Cols>, Rows> m_data;
	};

	// Quaternion class implementation
	template<typename T> class Quaternion {
	  public:
		Quaternion()
			: m_x(T{})
			, m_y(T{})
			, m_z(T{})
			, m_w(T{1}) {}
		Quaternion(T x, T y, T z, T w)
			: m_x(x)
			, m_y(y)
			, m_z(z)
			, m_w(w) {}
		explicit Quaternion(const Vec<T, 3>& euler);
		explicit Quaternion(const Vec<T, 3>& axis, T angle);

		// Basic operations
		Quaternion operator*(const Quaternion& other) const {
			return Quaternion(m_w * other.m_x + m_x * other.m_w + m_y * other.m_z - m_z * other.m_y,
			                  m_w * other.m_y - m_x * other.m_z + m_y * other.m_w + m_z * other.m_x,
			                  m_w * other.m_z + m_x * other.m_y - m_y * other.m_x + m_z * other.m_w,
			                  m_w * other.m_w - m_x * other.m_x - m_y * other.m_y - m_z * other.m_z);
		}

		Quaternion operator*(T scalar) const { return Quaternion(m_x * scalar, m_y * scalar, m_z * scalar, m_w * scalar); }
		Quaternion operator+(const Quaternion& other) const { return Quaternion(m_x + other.m_x, m_y + other.m_y, m_z + other.m_z, m_w + other.m_w); }
		Quaternion operator-(const Quaternion& other) const { return Quaternion(m_x - other.m_x, m_y - other.m_y, m_z - other.m_z, m_w - other.m_w); }

		Quaternion& operator+=(const Quaternion& other) {
			m_x += other.m_x;
			m_y += other.m_y;
			m_z += other.m_z;
			m_w += other.m_w;
			return *this;
		}

		Quaternion& operator-=(const Quaternion& other) {
			m_x -= other.m_x;
			m_y -= other.m_y;
			m_z -= other.m_z;
			m_w -= other.m_w;
			return *this;
		}

		Quaternion& operator*=(T scalar) {
			m_x *= scalar;
			m_y *= scalar;
			m_z *= scalar;
			m_w *= scalar;
			return *this;
		}

		Quaternion& operator*=(const Quaternion& other) {
			T x = m_w * other.m_x + m_x * other.m_w + m_y * other.m_z - m_z * other.m_y;
			T y = m_w * other.m_y - m_x * other.m_z + m_y * other.m_w + m_z * other.m_x;
			T z = m_w * other.m_z + m_x * other.m_y - m_y * other.m_x + m_z * other.m_w;
			T w = m_w * other.m_w - m_x * other.m_x - m_y * other.m_y - m_z * other.m_z;

			m_x = x;
			m_y = y;
			m_z = z;
			m_w = w;

			return *this;
		}

		Quaternion operator-() const { return Quaternion(-m_x, -m_y, -m_z, -m_w); }

		bool operator==(const Quaternion& other) const { return m_x == other.m_x && m_y == other.m_y && m_z == other.m_z && m_w == other.m_w; }

		bool operator!=(const Quaternion& other) const { return !(*this == other); }

		Vec<T, 3> RotateVector(const Vec<T, 3>& v) const {
			Vec<T, 3> q_vec(m_x, m_y, m_z);
			Vec<T, 3> t = q_vec.Cross(v) * T{2};
			return v + t * m_w + q_vec.Cross(t);
		}

		T          Length() const { return std::sqrt(m_x * m_x + m_y * m_y + m_z * m_z + m_w * m_w); }
		Quaternion Normalized() const {
			T len = Length();
			return len == T{} ? *this : *this * (T{1} / len);
		}
		Quaternion   Conjugate() const { return Quaternion(-m_x, -m_y, -m_z, m_w); }
		Mat<T, 3, 3> ToMat3() const;
		Mat<T, 4, 4> ToMat4() const;
		Vec<T, 3>    ToEuler() const;

		T& X() { return m_x; }
		T& Y() { return m_y; }
		T& Z() { return m_z; }
		T& W() { return m_w; }

		
		T X() const { return m_x; }
		T Y() const { return m_y; }
		T Z() const { return m_z; }
		T W() const { return m_w; }

	  private:
		T m_x, m_y, m_z, m_w;
	};

	// Common type aliases
	using Vec2f = Vec<f32, 2>;
	using Vec2d = Vec<double, 2>;
	using Vec2i = Vec<i32, 2>;
	using Vec2u = Vec<u32, 2>;

	using Vec3f = Vec<f32, 3>;
	using Vec3d = Vec<double, 3>;
	using Vec3i = Vec<i32, 3>;
	using Vec3u = Vec<u32, 3>;

	using Vec4f = Vec<f32, 4>;
	using Vec4d = Vec<double, 4>;
	using Vec4i = Vec<i32, 4>;
	using Vec4u = Vec<u32, 4>;

	using Mat2f = Mat<f32, 2, 2>;
	using Mat3f = Mat<f32, 3, 3>;
	using Mat4f = Mat<f32, 4, 4>;

	using Mat2d = Mat<double, 2, 2>;
	using Mat3d = Mat<double, 3, 3>;
	using Mat4d = Mat<double, 4, 4>;

	using Quatf = Quaternion<f32>;
	using Quatd = Quaternion<double>;

	using Vec2  = Vec2f;
	using Vec3  = Vec3f;
	using Vec4  = Vec4f;

	using Mat2  = Mat2f;
	using Mat3  = Mat3f;
	using Mat4  = Mat4f;

	using Quat  = Quatf;
} // namespace Spark

#endif // SPARK_MATH_TYPES_HPP