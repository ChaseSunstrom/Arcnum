#ifndef SPARK_RESOURCE_HPP
#define SPARK_RESOURCE_HPP

#include <core/pch.hpp>

namespace Spark {
	class IResourceContainer {
	  public:
		virtual ~IResourceContainer() = default;
	};

	template<typename T> class ResourceContainer : public IResourceContainer {
	  public:
		T&       Get() { return m_value; }
		const T& Get() const { return m_value; }
		void     Set(T&& value) { m_value = Move(value); }

	  private:
		T m_value;
	};
}

#endif