#ifndef SPARK_GAME_OBJECT_HPP
#define SPARK_GAME_OBJECT_HPP

#include <core/pch.hpp>
#include <core/scene/transform.hpp>

namespace Spark
{
	struct GameObject
	{
		// References to already existing data
		const std::string& m_model_name;
		const Transform& m_transform;
	};
}


#endif