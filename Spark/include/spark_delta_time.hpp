#ifndef SPARK_DELTA_TIME_HPP
#define SPARK_DELTA_TIME_HPP

#include "spark_pch.hpp"

namespace spark
{
	template <typename T>
	class DeltaTime
	{
		static_assert(std::is_floating_point_v<T>, "DeltaTime<T> must be a floating point value!");
	public:
		DeltaTime() : m_fixed(1.0 / 60.0), m_relative(0.0) { }

		DeltaTime(T fixed, T relative)
			: m_fixed(fixed), m_relative(relative)
		{
		}

		template <typename U>
		DeltaTime(const DeltaTime<U>& other)
			: m_fixed(static_cast<T>(other.GetFixed())), m_relative(static_cast<T>(other.GetRelative()))
		{
		}

		T GetFixed() const { return m_fixed; }
		T GetRelative() const { return m_relative; }

		void SetFixed(T fixed) { m_fixed = fixed; }
		void SetRelative(T relative) { m_relative = relative; }

		void Set(T fixed, T relative)
		{
			m_fixed = fixed;
			m_relative = relative;
		}

		void CalculateRelative(T delta)
		{
			m_relative = delta * m_fixed;
		}

	private:
		T m_fixed;
		T m_relative;
	};
}


#endif