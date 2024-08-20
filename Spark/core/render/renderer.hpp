#ifndef SPARK_RENDERER_HPP
#define SPARK_RENDERER_HPP

#include <core/pch.hpp>
#include <core/ecs/entity.hpp>
#include <core/scene/transform.hpp>

namespace Spark
{
	class Renderer
	{
	public:
		Renderer() = default;
		~Renderer() = default;

		virtual void Render(const Entity& entity) = 0;
		void AddRenderable(const std::string& model_name, const Transform& transform);
		void RemoveRenderable(const Transform& transform);
		void RemoveRenderables(const std::string& model_name);
		void ClearRenderables();
	private:
		// String of the model name the transform is using
		std::unordered_map<std::string, Transform> m_renderables;
	};
}


#endif