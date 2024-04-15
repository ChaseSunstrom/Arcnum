#ifndef SPARK_SINGELTON_HPP
#define SPARK_SINGELTON_HPP

#include "../spark.hpp"

namespace spark
{
	template <typename T>
	class singelton
	{
	public:
		static T& get_instance()
		{
			return *m_instance.get();
		}

	protected:
		singelton() { }  // Constructor needs to be accessible to derived classes.
		virtual ~singelton() { }

	private:
		singelton(const singelton&) = delete;

		singelton& operator=(const singelton&) = delete;

		static std::unique_ptr <T> create_instance()
		{
			return std::unique_ptr<T>(new T());
		}

		static std::unique_ptr <T> m_instance;

		static struct initializer
		{
			initializer()
			{
				singelton<T>::m_instance = create_instance();
			}
		} m_initializer;
	};
}

#endif