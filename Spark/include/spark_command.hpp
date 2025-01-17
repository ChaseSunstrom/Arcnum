#ifndef SPARK_COMMAND_HPP
#define SPARK_COMMAND_HPP

#include "spark_pch.hpp"

namespace spark
{
	struct ICommand
	{
		virtual ~ICommand() = default;
	};
}

#endif