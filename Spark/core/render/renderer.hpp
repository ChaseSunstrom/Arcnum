#ifndef SPARK_RENDERER_HPP
#define SPARK_RENDERER_HPP

#include <core/pch.hpp>
#include <core/ecs/entity.hpp>
#include <core/api.hpp>
#include <core/scene/transform.hpp>

namespace Spark
{
	class Renderer
	{
	public:
		Renderer(GraphicsAPI gapi) : m_gapi(gapi) {}
		~Renderer() = default;

		virtual void Render() = 0;
		void AddRenderable(const std::string& model_name, const Transform& transform);
		void RemoveRenderable(const Transform& transform);
		void RemoveRenderables(const std::string& model_name);
		void ClearRenderables();
	protected:
		GraphicsAPI m_gapi;
		// String of the model name the transform is using
		std::unordered_map<std::string, Transform> m_renderables;
	};

	template <typename T>
	concept IsRenderer = std::derived_from<T, Renderer>;
}


#endif