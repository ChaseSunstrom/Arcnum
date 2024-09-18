#ifndef SPARK_RENDERER_HPP
#define SPARK_RENDERER_HPP

#include <core/api.hpp>
#include <core/ecs/entity.hpp>
#include <core/pch.hpp>
#include <core/scene/scene.hpp>
#include <core/scene/transform.hpp>
#include <core/window/framebuffer.hpp>
#include <core/resource/resource.hpp>
#include "camera.hpp"

namespace Spark {
	class Renderer {
	  public:
		Renderer(GraphicsAPI gapi, Framebuffer& framebuffer, Manager<Resource>& resource_manager)
			: m_gapi(gapi)
			, m_framebuffer(framebuffer)
			, m_resource_manager(resource_manager) {}
		virtual ~Renderer()                     = default;
		virtual void Render(ConstPtr<Scene> scene) = 0;

	  protected:
		GraphicsAPI        m_gapi;
		Framebuffer&       m_framebuffer;
		Manager<Resource>& m_resource_manager;
	};

	template<typename _Ty>
	concept IsRenderer = std::derived_from<_Ty, Renderer>;
} // namespace Spark

#endif