#ifndef SPARK_CONTROL_HPP
#define SPARK_CONTROL_HPP

#include "../spark.hpp"

namespace spark
{
	enum class control_state
	{
		UNCONTROLLED = 0,
		CONTROLLED
	};

	struct control_component
	{
		control_component() = default;

		control_state m_control_state = control_state::UNCONTROLLED;
	};
}

#endif