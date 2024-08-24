#ifndef SPARK_SCENE_HPP
#define SPARK_SCENE_HPP

#include <core/pch.hpp>
#include "octree.hpp"

namespace Spark
{
	class Scene
	{
	public:

	private:
		// TODO: Implement storing of current conditionally rendered renderables
		std::unique_ptr<Octree> m_octree;
	};
}


#endif