#ifndef SPARK_ENGINE_HPP
#define SPARK_ENGINE_HPP

namespace spark
{
	class engine
	{
	public:
		// For getting class singeltons
		// THESE CLASSES MUST IMPLEMENT THE get() METHOD
		template <typename T>
		static T& get()
		{
			return T::get();
		}
	};
}

#endif