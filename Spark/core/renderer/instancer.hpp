#ifndef SPARK_INSTANCER_HPP
#define SPARK_INSTANCER_HPP

#include "../ecs/ecs.hpp"

namespace spark
{
	class instancer
	{
		
	private:
		// stores all current renderable entities:
		// mesh_name -> material_name -> transforms
		std::unordered_map<std::string, std::unordered_map<std::string, std::vector<transform_component>>> m_renderables;
	};
}

#endif