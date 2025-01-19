#ifndef SPARK_COMMAND_HPP
#define SPARK_COMMAND_HPP

#include "spark_pch.hpp"

namespace spark
{
	struct ICommand
	{
		virtual ~ICommand() = default;
		virtual void Execute(const std::function<void(ICommand&)>& fn) = 0;
	};


	template <typename CTy>
	concept ValidCommand = std::is_base_of_v<ICommand, CTy>;
}

#endif